/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:30:43 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/14 18:15:08 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(void) {}

Socket::Socket(std::string port_):
	StrPort(port_),
	address("0.0.0.0"),
	ai(NULL)
{ }

Socket::Socket(const std::string &address, const std::string &port):
  StrPort(port),
	address(address),
	ai(NULL)
{ }

Socket::~Socket(void)
{ }

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
		port = obj.port;
		hints = obj.hints;
		ai = obj.ai;
		address = obj.address;
		server = obj.server;
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

int	Socket::set_sockaddr_in()
{
	int res;

	memset(&hints, 0, sizeof(hints));
	// hints.ai_family = AF_INET6; /* with IPv6 rule */
	hints.ai_family = AF_UNSPEC; /* with IPv4 and IPv6 rule */
	hints.ai_socktype = SOCK_STREAM; /* TCP socket type */
	hints.ai_flags = AI_PASSIVE; /* In using bind case, requied*/
	// getaddrinfo(NULL, StrPort.c_str(), &hints, &ai); //第四引数にaddrinfoの結果が格納される
	res = getaddrinfo(this->address.c_str(), StrPort.c_str(), &hints, &ai);
	// std::cout << this->address << ":" << this->StrPort << ", " << res << std::endl;
	// std::cout << ai->ai_next << std::endl;
	return (res);
}

int		Socket::set_socket()
{
	int	optval = 1;
	int	ret_so;
	int	ret_bi;
	int ret_lis;
	int res;

	//ソケット通信方式の設定
	if ((res = Socket::set_sockaddr_in()) != 0) {
		std::cerr << res << ":" << gai_strerror(res) << std::endl;
		freeaddrinfo(ai);
		return (-1);
	}
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
		freeaddrinfo(ai);
		return (-1);
	}
	//bindでソケットを登録。bindすることでlistenfdにポートが割り当てられる
	ret_bi = bind(listenfd,	ai->ai_addr, ai->ai_addrlen);
	if (ret_bi == -1)
	{
		std::cerr << std::strerror(errno) << ":";
		std::cerr << "bind() failed." << std::endl;
		close(listenfd);
		freeaddrinfo(ai);
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
		freeaddrinfo(ai);
		return (-1);
	}
	//ソケットアドレスのリストを解放する必要がある
	this->port = webservconfig::get_in_port(ai->ai_addr);
	freeaddrinfo(ai);
	return (0);
}

int		Socket::get_listenfd() const
{
	return (listenfd);
}

void Socket::set_server(const webservconfig::Server &s) { this->server = s; }

const std::string &Socket::get_StrPort() const { return (this->StrPort); }
int Socket::get_port() const { return (this->port); }
const std::string &Socket::get_address() const { return (this->address); }
const webservconfig::Server &Socket::get_server() const { return (this->server); }
// const struct addrinfo *Socket::get_ai() const { return (this->ai); }
