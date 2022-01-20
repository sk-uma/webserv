/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:40:53 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/20 13:50:40 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Setting.hpp"
#include "Socket.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ServerCollection.hpp"
#include "SocketCollection.hpp"
#include "ClientManage.hpp"
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
		if (config.GetServer().size() == 0) {
			throw std::runtime_error("Cannot find the server directive");
		}
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
	std::cout << "listen:" << std::endl;
	for (std::vector<Socket>::const_iterator it = sock.begin(); it != sock.end(); it++) {
		std::cout << it->get_address() << ":" << it->get_port() << std::endl;
	}
	std::cout << std::endl;

	//accfdは使用するファイルディスクリプタチェック
	//rfdは読み取り可能ファイルディスクリプタ登録用
	//wfdは書き込み可能ファイルディスクリプタ登録用
	//manageはClient管理用のクラス
	int					accfd[MAX_SESSION];
	fd_set				rfd;
	fd_set				wfd;
	ClientManage		manage;

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

		//listenfdから接続要求を取り出して参照する新しいファイルディスクリプターを設定する
		for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
		{
			if (FD_ISSET((*it).get_listenfd(), &rfd))
			{
				int connfd = accept((*it).get_listenfd(), (struct sockaddr*)NULL, NULL);
				if (connfd == -1)
					continue ;
				fcntl(connfd, F_SETFL, O_NONBLOCK);
				bool limit_over = true;
				for (int i = 0; i < MAX_SESSION; i++)
				{
					if (accfd[i] == -1)
					{
						accfd[i] = connfd;
						manage.Init(accfd[i], it->get_server());
//						std::cout << "Accept: " << it->get_address() << ":" << it->get_StrPort() << std::endl;
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

			if (FD_ISSET(accfd[i], &rfd))
			{
				std::string	req_str = "";
				char *req_buf;
				req_buf = new char[REQUEST_SIZE]; //ヒープの方が大きなサイズで設定可能
				ssize_t		read_size = read(accfd[i], req_buf, sizeof(char[REQUEST_SIZE]));
				if (read_size <= 0)
				{
					std::cout << "read():error" << std::endl;
					manage.Erase(accfd[i]);
					close(accfd[i]);
					accfd[i] = -1;
					delete[] req_buf;
					continue ;
				}
				req_buf[read_size] = '\0';
				//画像などバイナリで処理対応。一文字ずつ取得。
				for (int i = 0; i < read_size; ++i)
        			req_str += req_buf[i];
				manage.AppendReq(accfd[i], req_str);
				delete[] req_buf;
			}
			else if (FD_ISSET(accfd[i], &wfd))
			{
				//Requestになにも取得していない状態の場合、writeしない
				if (manage.GetReq(accfd[i]) == "")
					continue ;
				RequestParser 	request(manage.GetReq(accfd[i]), manage.GetConf(accfd[i]));
				//Content-Lengthがあるが、bodyが取得できていない場合、writeしない
				if ((unsigned long)atoi(request.get_content_length().c_str()) != request.get_body().length())
					continue ;
				Response		response(request, manage.GetConf(accfd[i]));
				std::string		response_str;
				
				//method HEADの場合はヘッダーだけ出力
				if (request.get_method() == "HEAD")
					response_str = response.get_header();
				else
					response_str = response.get_header() + response.get_body();

				unsigned long	chunk_size = RESPONSE_BUFFER_SIZE;
				if (response_str.size() - manage.GetResSize(accfd[i]) < chunk_size)
					chunk_size = response_str.size() - manage.GetResSize(accfd[i]);

				ssize_t			write_size = write(accfd[i], response_str.c_str() + manage.GetResSize(accfd[i]), chunk_size);
				if (g_SIGPIPE_FLAG == 1)
				{
					std::cout << "SIGPIPE" << std::endl;
					g_SIGPIPE_FLAG = 0;
				}
				else if (write_size <= 0)
					std::cout << "write():error" << std::endl;
				else if (write_size > 0)
				{
					manage.AppendResSize(accfd[i], write_size);
					if (manage.GetResSize(accfd[i]) != response_str.size())
						continue ;
				}

				//デバッグ用。Config出力
				//PutConf(manage.GetConf(accfd[i]), request);
				
				manage.Erase(accfd[i]);
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