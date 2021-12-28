/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:31:14 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/27 23:34:02 by rtomishi         ###   ########.fr       */
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
		std::string       address;
		struct addrinfo		hints;
		struct addrinfo		*ai;
		std::vector<webservconfig::Server>  server;

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

		int		get_listenfd() const;
		const std::string &get_port() const;
		const std::string &get_address() const;
		const struct addrinfo *get_ai() const;

		void add_server(const webservconfig::Server &s);
};

#endif
