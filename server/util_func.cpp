/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Util_func.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/26 22:20:24 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/05 10:39:15 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Setting.hpp"

//環境変数EXE_DIRに実行ファイルのディレクトリを格納する
void	setenv_exedir(char **argv)
{
	std::string	exepath(argv[0]);
	std::size_t	pos;

	pos = exepath.rfind("/");
	setenv("EXE_DIR", exepath.substr(0, pos).c_str(), 1);
}

std::vector<std::string> PortVec(webservconfig::ServerCollection &config)
{
	std::vector<std::string>	ret_vec;

	for (std::vector<webservconfig::Server>::const_iterator it = config.GetServer().begin(); it != config.GetServer().end(); it++)
	{
		// for (webservconfig::ServerCollection::listen_type::const_iterator sit = (*it).GetListenV4().begin(); sit != (*it).GetListenV4().end(); sit++)
		// {
		// 	std::stringstream	ss;
		// 	ss << (*sit).second;
		// 	ret_vec.push_back(ss.str());
		// }
	}
	return (ret_vec);
}

