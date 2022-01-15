/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:40:53 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/15 23:55:48 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Setting.hpp"
#include "Socket.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ServerCollection.hpp"
#include "SocketCollection.hpp"
#include "Prototype.hpp"

//SIGPIPE対応用フラグ
int		g_SIGPIPE_FLAG = 0;

//SIGPIPE対応の関数。フラグを上げる。
void	sigpipe_handler(int sig)
{
	(void)sig;
	g_SIGPIPE_FLAG = 1;
}

//SIGPIPEが来た時のセッティング。
void	sigpipe_wait(void)
{
	struct sigaction	act;

	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = sigpipe_handler;
	sigaction(SIGPIPE, &act, NULL);
}

int	main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "usage: webserv [config file path]" << std::endl;
		return (-1);
	}
	webservconfig::ServerCollection config;
	SocketCollection socket_c;
	try {
		config = webservconfig::ServerCollection(argv[1]);
		socket_c = SocketCollection(config);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	//SIGPIPEスルー対応
	sigpipe_wait();
	//環境変数EXE_DIRに実行ファイルのディレクトリを格納する
	setenv_exedir(argv);

	std::vector<Socket> sock = socket_c.GetSocket();
	// std::cout << "fin sock set: " << sock.size() << std::endl;
	for (std::vector<Socket>::const_iterator it = sock.begin(); it != sock.end(); it++) {
		std::cout << it->get_address() << ":" << it->get_port() << std::endl;
	}
	std::cout << std::endl;

	//accfdは使用するファイルディスクリプタチェック
	//rfdは読み取り可能ファイルディスクリプタ登録用
	//wfdは書き込み可能ファイルディスクリプタ登録用
	int					accfd[MAX_SESSION];
	fd_set				rfd;
	fd_set				wfd;
	std::map<int, webservconfig::Server>	serv_map;
	std::map<int, std::string>				req_map;
	std::map<int, unsigned long>			res_size_map;

	for (int i = 0; i < MAX_SESSION; i++)
		accfd[i] = -1;

	//FD_SETで監視対象のディスクリプタをセットする
	while (1)
	{
		int		nfd = (*(sock.end() - 1)).get_listenfd() + 1;

		FD_ZERO(&rfd);
		FD_ZERO(&wfd);
		for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
		{
			FD_SET((*it).get_listenfd(), &rfd);
			FD_SET((*it).get_listenfd(), &wfd);
		}
		for (int i = 0; i < MAX_SESSION; i++)
		{
			if (accfd[i] != -1)
			{
				FD_SET(accfd[i], &rfd);
				FD_SET(accfd[i], &wfd);
				if (nfd < accfd[i] + 1)
					nfd = accfd[i] + 1;
			}
		}
		//rfd, wfdに登録されているファイルディスクリプタにアクションがくるまで待機する
		if (select(nfd, &rfd, &wfd, NULL, NULL) == -1)
			std::cerr << "select() failed." << std::endl;

//		std::cout << "+++++++++++++++++++++++++\n";
//		for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
//			std::cout << "fd:" << (*it).get_listenfd() << " rfd:" << FD_ISSET((*it).get_listenfd(), &rfd) << " wfd:" << FD_ISSET((*it).get_listenfd(), &wfd) << std::endl;
//	
//		for (int i = 0; i < MAX_SESSION; i++)
//			std::cout << "accfd:" << accfd[i] << " rfd:" << FD_ISSET(accfd[i], &rfd) << " wfd:" << FD_ISSET(accfd[i], &wfd) << std::endl;;
//
		//listenfdから接続要求を取り出して参照する新しいファイルディスクリプターを設定する
		for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
		{
			if (FD_ISSET((*it).get_listenfd(), &rfd))
			{
				int connfd = accept((*it).get_listenfd(), (struct sockaddr*)NULL, NULL);
				fcntl(connfd, F_SETFL, O_NONBLOCK);
				bool limit_over = true;
				for (int i = 0; i < MAX_SESSION; i++)
				{
					if (accfd[i] == -1)
					{
						accfd[i] = connfd;
						serv_map.insert(std::make_pair(connfd, it->get_server()));
						req_map.insert(std::make_pair(connfd, ""));
						res_size_map.insert(std::make_pair(connfd, 0));
						std::cout << "Accept: " << it->get_address() << ":" << it->get_StrPort() << std::endl;
						limit_over = false;
						break;
					}
				}
				if (limit_over)
				{
					close(connfd);
					std::cout << "over max connection." << std::endl;
				}
			}
		}
		//アクションがあったファイルディスクリプタのリクエストに対し、レスポンスを返す
		for (int i = 0; i < MAX_SESSION; i++)
		{
			if (accfd[i] == -1)
				continue ;

			std::string	req_str = "";
			//if (FD_ISSET(accfd[i], &rfd) && FD_ISSET(accfd[i], &wfd))
			if (FD_ISSET(accfd[i], &rfd))
			{
				char *req_buf;
				req_buf = new char[REQUEST_SIZE]; //ヒープの方が大きなサイズで設定可能
				ssize_t		read_size = read(accfd[i], req_buf, sizeof(char[REQUEST_SIZE]));
				if (read_size <= 0)
				{
					std::cout << "read():error" << std::endl;
					serv_map.erase(accfd[i]);
					req_map.erase(accfd[i]);
					res_size_map.erase(accfd[i]);
					close(accfd[i]);
					accfd[i] = -1;
					delete[] req_buf;
					continue ;
				}
				req_buf[read_size] = '\0';
				//画像などバイナリで処理対応。一文字ずつ取得。
				for (int i = 0; i < read_size; ++i)
        			req_str += req_buf[i];
				req_map[accfd[i]] += req_str;
				delete[] req_buf;
			}
			else if (FD_ISSET(accfd[i], &wfd))
			{
				RequestParser 	request(req_map[accfd[i]], serv_map[accfd[i]]);
				Response		response(request, serv_map[accfd[i]]);
				std::string		response_str;

				//method HEADの場合はヘッダーだけ出力
				if (request.get_method() == "HEAD")
					response_str = response.get_header();
				else
					response_str = response.get_header() + response.get_body();

				unsigned long	chunk_size = RESPONSE_BUFFER_SIZE;
				if (response_str.size() - res_size_map[accfd[i]] < chunk_size)
					chunk_size = response_str.size() - res_size_map[accfd[i]];

				//write一回だけのケース
				ssize_t			write_size = write(accfd[i], response_str.c_str() + res_size_map[accfd[i]], chunk_size);
				if (g_SIGPIPE_FLAG == 1)
				{
					std::cout << "SIGPIPE" << std::endl;
					g_SIGPIPE_FLAG = 0;
				}
				else if (write_size <= 0)
					std::cout << "write():error" << std::endl;
				res_size_map[accfd[i]] += write_size;
				if (res_size_map[accfd[i]] != response_str.size())
					continue ;

				//デバッグ用。Config出力
				PutConf(serv_map[accfd[i]], request);

				serv_map.erase(accfd[i]);
				req_map.erase(accfd[i]);
				res_size_map.erase(accfd[i]);
				close(accfd[i]);
				accfd[i] = -1;
			}
		}
	}
	//基本的にここには到達しない。
	for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
		close((*it).get_listenfd());
	return (0);
}