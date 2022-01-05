/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:31:14 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/03 23:24:21 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Setting.hpp"
# include "ServerCollection.hpp"
# include "Server.hpp"


class Socket
{
	private:
		int								listenfd;
		std::string				StrPort;
		int 							port;
		std::string       address;
		struct addrinfo		hints;
		struct addrinfo		*ai;
		// std::vector<webservconfig::Server>  server;
		webservconfig::Server server;

	public:
		Socket(void);
		explicit Socket(std::string port_);
		explicit Socket(const std::string &address, const std::string &port);
		~Socket(void);
		Socket (Socket const &copy);
		Socket &operator=(Socket const &obj);

/**
 * Setter
 */

		void	set_listenfd();
		int		set_sockaddr_in();
		int		set_socket();

/**
 * Getter
 */

		int								get_listenfd() const;
		const std::string &get_StrPort() const;
		int 							get_port() const;
		const std::string &get_address() const;
		const webservconfig::Server &get_server() const;
		// const struct addrinfo *get_ai() const;

		void set_server(const webservconfig::Server &s);
};

#endif
