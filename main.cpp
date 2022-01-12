/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:40:53 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/12 23:08:27 by rtomishi         ###   ########.fr       */
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
		{
			std::cerr << "select() failed." << std::endl;
			break;
		}
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
			char buf[BUF_SIZE + 1];
			memset(buf, 0, sizeof(buf));
			if (FD_ISSET(accfd[i], &rfd) && FD_ISSET(accfd[i], &wfd))
			{
				std::string	recv_str = "";
				ssize_t		read_size = 0;
				
				//Request
				while (read_size >= 0)
				{
					read_size = read(accfd[i], buf, sizeof(buf));
					if (read_size > 0)
					{
						buf[read_size] = '\0';
						//画像などバイナリで処理対応。一文字ずつ取得。
						for (int i = 0; i < read_size; ++i)
        					recv_str += buf[i];
					}
					//読み込みが空の場合はスルーする
					else if (read_size == 0)
					{
						//std::cerr << "read_size 0" << std::endl;
						serv_map.erase(accfd[i]);
						close(accfd[i]);
						accfd[i] = -1;
						break ;
					}
					//ヘッダー情報がまだ読み込めていない場合、ループ継続。
					if (read_size == -1 && recv_str.find("\r\n\r\n") == std::string::npos)
					{
						read_size = 0;
						continue ;
					}
					//Content-Typeがあるにもかかわらず、ヘッダーの終わり状態の場合、ループ継続。
					else if (read_size == -1 && recv_str.find("Content-Type") != std::string::npos && recv_str.find("\r\n\r\n") == recv_str.length() - 4)
					{
						read_size = 0;
						continue ;						
					}
					//ファイルアップロードでFオプションの対応
					else if (read_size == -1 && recv_str.find("Content-Type: multipart/form-data;") != std::string::npos)
					{
						std::istringstream	iss(recv_str);
						std::string			line;
						std::string			str;
						std::size_t			epos;
						
						while (getline(iss, line))
						{
							if (line.find("Content-Type: multipart/form-data;") != std::string::npos)
							{
								epos = line.find("=");
								str = line.substr(epos + 1, line.length() - (epos + 1 + 1));
							}
						}
						if (recv_str.rfind(str + "--\r\n") == std::string::npos)
						{
							read_size = 0;
							continue ;
						}
					}
				}
				if (read_size == 0)
					break ;

				RequestParser 	request(recv_str, serv_map[accfd[i]]);
				Response		response(request, serv_map[accfd[i]]);
				std::string		response_str;
				ssize_t			write_size = 0;
				ssize_t			ret_size;
				std::size_t		chunk_size = RESPONSE_BUFFER_SIZE;

				//デバッグ用。Config出力
				PutConf(serv_map[accfd[i]], request);

				//method HEADの場合はヘッダーだけ出力
				if (request.get_method() == "HEAD")
					response_str = response.get_header();
				else
					response_str = response.get_header() + response.get_body();
				while (write_size >= 0)
				{
					if (response_str.size() - write_size < chunk_size)
						chunk_size = response_str.size() - write_size;
					ret_size = write(accfd[i], response_str.c_str() + write_size, chunk_size);
					//ソケットが壊れたときにSIGPIPEが送られる。プロセスを終了させないために対応
					if (g_SIGPIPE_FLAG == 1)
					{
						g_SIGPIPE_FLAG = 0;
						break ;
					}
					if (ret_size > 0)
						write_size += ret_size;
					if (write_size == (long)response_str.size())
						break ;
				}
				serv_map.erase(accfd[i]);
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