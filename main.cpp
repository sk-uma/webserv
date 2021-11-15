/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:40:53 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/15 16:59:10 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "socket.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"

int	main(void)
{
	//複数のポートを使用できるようにvector<Socket> sockを作成する
	//現状、お試しでvector<string> ports を作成してから、
	//for ループで各ポートのSocketのインスタンスをvectorでつくる
	std::vector<std::string>	ports;
	std::vector<Socket>			sock;
	ports.push_back("5050");
	ports.push_back("6060");
	ports.push_back("7070");
	for (std::vector<std::string>::iterator	it = ports.begin(); it != ports.end(); it++)
		sock.push_back(Socket(*it));
	//各ポートのソケットのセットアップをする
	//生成されたソケットのファイルディスクリプタをfcntlでノンブロッキングに設定する
	for (std::vector<Socket>::iterator it = sock.begin(); it != sock.end(); it++)
	{
		(*it).set_socket();
		fcntl((*it).get_listenfd(), F_SETFL, O_NONBLOCK);
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

					if (read_size > 0)
					{
						buf[read_size] = '\0';
						recv_str.append(buf);
					//	std::cout << "read_size:" << read_size << std::endl;
					//	std::cout << "[buf]\n" << buf << std::endl;
					}
					else if (read_size == 0)
					{
						std::cerr << "read() failed." << std::endl;
						close(accfd[i]);
						accfd[i] = -1;
						break ;
					}			
					if (read_size == -1 && recv_str.find("\r\n\r\n") == std::string::npos)
					{
						read_size = 0;
						continue ;
					}
					else if (read_size == -1 && recv_str.find("Content-Type") != std::string::npos && recv_str.find("\r\n\r\n") == recv_str.length() - 4)
					{
						read_size = 0;
						continue ;						
					}
				}
				if (read_size == 0)
					break ;
				//Response
				//std::cout << "[recv_str]\n" << recv_str << std::endl;
				//std::cout << "recv_length:" << recv_str.length() << std::endl;
				RequestParser 	request(recv_str);
				Response		response(request);

				std::string		response_str = response.get_header() + response.get_body();
				ssize_t			write_size = 0;

				while (write_size >=0)
				{
					write_size += write(accfd[i], response_str.c_str(),
									response_str.length());
					if (write_size == (long)response_str.length())
						break ;
					else if (write_size == -1)
					{
				//		std::cout << "errorno:" << strerror(errno) << std::endl;
						std::cerr << "write() failed." << std::endl;
						break ;
					}
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