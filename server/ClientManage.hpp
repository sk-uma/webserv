/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/16 21:03:42 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/16 22:09:48 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTMANAGE_HPP
# define CLIENTMANAGE_HPP

#include "ServerCollection.hpp"
#include "Setting.hpp"

class ClientManage
{
	private:
		std::map<int, webservconfig::Server>	conf;
		std::map<int, std::string>				req;
		std::map<int, unsigned long>			res_size;

	public:
		ClientManage(void);
		~ClientManage(void);
		ClientManage(ClientManage const &copy);
		ClientManage &operator=(ClientManage const &obj);

		void Init(int fd, webservconfig::Server serv);
		void SetConf(int fd, webservconfig::Server serv);
		void AppendReq(int fd, std::string req_str);
		void AppendResSize(int fd, unsigned long size);

		webservconfig::Server &GetConf(int fd);
		std::string GetReq(int fd);
		unsigned long GetResSize(int fd);

		void Erase(int fd);
};

#endif
