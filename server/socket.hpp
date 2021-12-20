/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:31:14 by rtomishi          #+#    #+#             */
/*   Updated: 2021/10/03 09:58:09 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "config.hpp"

class Socket
{
	private:
		int					listenfd;
		std::string			StrPort;
		struct addrinfo		hints;
		struct addrinfo		*ai;
	
	public:
		Socket(void);
		explicit Socket(std::string port_);
		~Socket(void);
		Socket (Socket const &copy);
		Socket &operator=(Socket const &obj);
		
		void	set_listenfd();
		void	set_sockaddr_in();
		int		set_socket();
		int		get_listenfd() const;
};

#endif
