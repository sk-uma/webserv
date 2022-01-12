/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_func.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/26 22:20:24 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/12 23:06:42 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Setting.hpp"
#include "Socket.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "ServerCollection.hpp"
#include "SocketCollection.hpp"

//環境変数EXE_DIRに実行ファイルのディレクトリを格納する
void	setenv_exedir(char **argv)
{
	std::string	exepath(argv[0]);
	std::size_t	pos;

	pos = exepath.rfind("/");
	setenv("EXE_DIR", exepath.substr(0, pos).c_str(), 1);
}

//デバッグ用。Configを出力する関数
void	PutConf(webservconfig::Server	&serv, RequestParser &request)
{
	std::cout << "================ " << request.get_uri() << " ====================" << std::endl;
	webservconfig::ConfigBase::index_type	id = serv.GetIndex(request.get_uri());
	std::cout << "index:";
	for (size_t	i = 0; i < id.size(); i++)
		std::cout << id[i] << " ";
	std::cout << std::endl;

	webservconfig::ConfigBase::error_page_type	err = serv.GetErrorPage(request.get_uri());
	std::cout << "error_page:";
	for (webservconfig::ConfigBase::error_page_type::iterator	it = err.begin(); it != err.end(); it++)
		std::cout << (*it).first << "_" << (*it).second << " ";
	std::cout << std::endl;

	std::cout << "autoindex:" << serv.GetAutoIndex(request.get_uri()) << std::endl;

	std::cout << "body_size:" << serv.GetClientMaxBodySize(request.get_uri()) << std::endl;
	webservconfig::ConfigBase::limit_except_type	lim = serv.GetLimitExceptByDenyAll(request.get_uri());
	std::cout << "limit_except:";
	for (webservconfig::ConfigBase::limit_except_type::iterator	it = lim.begin(); it != lim.end(); it++)
		std::cout << (*it).first << "_" << (*it).second << " ";
	std::cout << std::endl;

	webservconfig::ConfigBase::return_type	ret = serv.GetReturn(request.get_uri());
	std::cout << "return:" << ret.first << "_" << ret.second << std::endl;

	std::cout << "UploadPath:" << serv.GetUploadPath(request.get_uri()) << std::endl;

	std::cout << "root:" << serv.GetRoot(request.get_uri()) << std::endl;

	webservconfig::ConfigBase::index_type	ex = serv.GetCgiExtension(request.get_uri());
	std::cout << "extension_list_type:";
	for (size_t	i = 0; i < ex.size(); i++)
		std::cout << ex[i] << " ";
	std::cout << std::endl;

	webservconfig::ConfigBase::server_name_list_type	na = serv.GetServerName();
	std::cout << "server_name:";
	for (size_t	i = 0; i < na.size(); i++)
		std::cout << na[i] << " ";
	std::cout << std::endl;
	std::cout << std::endl;
}
