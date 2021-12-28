/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:40:53 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/27 23:31:59 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Setting.hpp"
#include "Socket.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ServerCollection.hpp"
#include "SocketCollection.hpp"

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
	webservconfig::ServerCollection config(argv[1]);
	SocketCollection socket_c(config);

	sigpipe_wait();
	//環境変数EXE_DIRに実行ファイルのディレクトリを格納する
	setenv_exedir(argv);

	// std::cout << "in main" << std::endl;

	//複数のポートを使用できるようにvector<Socket> sockを作成する
	//現状、お試しでvector<string> ports を作成してから、
	//for ループで各ポートのSocketのインスタンスをvectorでつくる
	// std::vector<std::string>	ports;
	// std::vector<Socket>			sock;
	// ports.push_back("5050");
	// ports.push_back("6060");
	// ports.push_back("7070");
	// for (std::vector<std::string>::iterator	it = ports.begin(); it != ports.end(); it++)
	// 	sock.push_back(Socket(*it));
	//各ポートのソケットのセットアップをする
	//生成されたソケットのファイルディスクリプタをfcntlでノンブロッキングに設定する
	// for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
	// {
	// 	(*it).set_socket();
	// 	fcntl((*it).get_listenfd(), F_SETFL, O_NONBLOCK);
	// }
	std::vector<Socket> sock = socket_c.GetSocket();
	// std::cout << "fin sock set: " << sock.size() << std::endl;
	for (std::vector<Socket>::const_iterator it = sock.begin(); it != sock.end(); it++) {
		std::cout << it->get_address() << ":" << it->get_port() << std::endl;
	}

	//accfdは使用するファイルディスクリプタチェック
	//rfdは読み取り可能ファイルディスクリプタ登録用
	//wfdは書き込み可能ファイルディスクリプタ登録用
	int					accfd[MAX_SESSION];
	fd_set				rfd;
	fd_set				wfd;

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
			std::cout << "select() failed." << std::endl;
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
//					std::cout << "read_size:" << read_size << std::endl;
//					if (read_size < 0)
//						std::cout << "error:" << errono << ":" << strerror(errno) << std::endl;
					if (read_size > 0)
					{
//						std::cout << "situation:read_size > 0" << std::endl;
						buf[read_size] = '\0';
						//バイナリで処理したいため一文字ずつとする。対応しないと画像などのバイナリファイル対応できない。
						for (int i = 0; i < read_size; ++i)
        					recv_str += buf[i];
					//	std::cout << "read_size:" << read_size << std::endl;
					//	std::cout << "[buf]\n" << buf << std::endl;
					}
					else if (read_size == 0)
					{
//						std::cout << "situation:read_size = 0" << std::endl;
						std::cerr << "read_size 0" << std::endl;
						close(accfd[i]);
						accfd[i] = -1;
						break ;
					}			
					if (read_size == -1 && recv_str.find("\r\n\r\n") == std::string::npos)
					{
//						std::cout << "situation:read_size = -1 and CRLF not found" << std::endl;
						read_size = 0;
						continue ;
					}
					else if (read_size == -1 && recv_str.find("Content-Type") != std::string::npos && recv_str.find("\r\n\r\n") == recv_str.length() - 4)
					{
//						std::cout << "situation:read_size = -1 and Content-Type + tail is CRLF" << std::endl;
						read_size = 0;
						continue ;						
					}
					//ファイルアップロードでFオプションの対応
					else if (read_size == -1 && recv_str.find("Content-Type: multipart/form-data;") != std::string::npos)
					{
//						std::cout << "situation:read_size = -1 and multipart/form-data" << std::endl;
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
				//Response

//				for (unsigned long i = 0; i < recv_str.size(); ++i)
//        			std::cout << recv_str[i];
//				std::cout << "[recv_str]\n" << recv_str.c_str() << std::endl;
//				std::cout << "recv_length:" << recv_str.length() << std::endl;
//				std::cout << "recv_size:" << recv_str.size()*sizeof(std::string::value_type) << std::endl;
				RequestParser 	request(recv_str);
				Response		response(request);

				std::string		response_str;
				ssize_t			write_size = 0;
				ssize_t			ret_size;
				std::size_t		chunk_size = RESPONSE_BUFFER_SIZE;

				if (request.get_method() == "HEAD")
					response_str = response.get_header();
				else
					response_str = response.get_header() + response.get_body();
//				std::cout << "[response_header]\n" << response.get_header() << std::endl;
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
				close(accfd[i]);
				accfd[i] = -1;
			}
		}
	}
	for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
		close((*it).get_listenfd());
	return (0);
}