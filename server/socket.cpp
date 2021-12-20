/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:30:43 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/17 22:57:01 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"

Socket::Socket(void) {}

Socket::Socket(std::string port_):StrPort(port_), ai(NULL) {}

Socket::~Socket(void) {}

Socket::Socket (Socket const &copy)
{
	*this = copy;
}

Socket &Socket::operator=(Socket const &obj)
{
	if (this != &obj)
	{
		listenfd = obj.listenfd;
		StrPort = obj.StrPort;
		hints = obj.hints;
		ai = obj.ai;
	}
	return (*this);
}

/* func socket() initialized socket */
void	Socket::set_listenfd()
{
	/*Socket Generation:AF_INET-IPv6, SOCK_STREAM-TCP, third_arg:protocol*/
	listenfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (listenfd == -1)
	{
		std::cerr << std::strerror(errno) << ":";
		std::cerr << "socket() failed." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void	Socket::set_sockaddr_in()
{
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6; /* with IPv4 and IPv6 rule */
	hints.ai_socktype = SOCK_STREAM; /* TCP socket type */
	hints.ai_flags = AI_PASSIVE; /* In using bind case, requied*/
	getaddrinfo(NULL, StrPort.c_str(), &hints, &ai); //第四引数にaddrinfoの結果が格納される
}

int		Socket::set_socket()
{
	int	optval = 1;
	int	ret_so;
	int	ret_bi;
	int ret_lis;

	//ソケット通信方式の設定
	Socket::set_sockaddr_in();
	//addrinfoからソケットを生成する(待ち受けるファイルディスクリプタをつくる)
	Socket::set_listenfd();
	//ソケット関連のオプション設定
	/*socket option setting:SOL_SOCKET-operate with API layer（特別でない限り変えない）,
	 		SO_REUSEADDR-reuse socket address (TIME_WAIT状態のポートでbind可能)*/
	ret_so = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
	if (ret_so == -1)
	{
		std::cerr << std::strerror(errno) << ":";
		std::cerr << "setsockopt() failed." << std::endl;
		close(listenfd);
		return (-1);
	}
	//bindでソケットを登録。bindすることでlistenfdにポートが割り当てられる
	ret_bi = bind(listenfd,	ai->ai_addr, ai->ai_addrlen);
	if (ret_bi == -1)
	{
		std::cerr << std::strerror(errno) << ":";
		std::cerr << "bind() failed." << std::endl;
		close(listenfd);
		return (-1);
	}
	//listenでソケットを接続待ちにする
	/* SOMAXCONN:optimize max que length */
	ret_lis = listen(listenfd, SOMAXCONN);
	if (ret_lis == -1)
	{
		std::cerr << std::strerror(errno) << ":";
		std::cerr << "listen() failed." << std::endl;
		close(listenfd);
		return (-1);
	}
	//ソケットアドレスのリストを解放する必要がある
	freeaddrinfo(ai);
	return (0);
}

int		Socket::get_listenfd() const
{
	return (listenfd);
}
