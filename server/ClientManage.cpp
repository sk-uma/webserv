/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/16 21:12:23 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/16 22:09:57 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManage.hpp"

ClientManage::ClientManage(void) {};

ClientManage::~ClientManage(void) {};

ClientManage::ClientManage(ClientManage const &copy)
{
	*this = copy;	
}

ClientManage &ClientManage::operator=(ClientManage const &obj)
{
	if (this != &obj)
	{
		conf = obj.conf;
		req = obj.req;
		res_size = obj.res_size;
	}
	return (*this);
}

void ClientManage::Init(int fd, webservconfig::Server serv)
{
	conf[fd] = serv;
	req[fd] = "";
	res_size[fd] = 0;
}

void ClientManage::SetConf(int fd, webservconfig::Server serv) {conf[fd] = serv;}
void ClientManage::AppendReq(int fd, std::string req_str) {req[fd] += req_str;}
void ClientManage::AppendResSize(int fd, unsigned long size) {res_size[fd] += size;}
webservconfig::Server &ClientManage::GetConf(int fd) {return conf[fd];}
std::string ClientManage::GetReq(int fd) {return req[fd];}
unsigned long ClientManage::GetResSize(int fd) {return res_size[fd];}

void ClientManage::Erase(int fd)
{
	conf.erase(fd);
	req.erase(fd);
	res_size.erase(fd);
}

