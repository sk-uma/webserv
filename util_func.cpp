/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   util_func.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/26 22:20:24 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/19 00:08:30 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

//環境変数EXE_DIRに実行ファイルのディレクトリを格納する
void	setenv_exedir(char **argv)
{
	std::string	exepath(argv[0]);
	std::size_t	pos;

	pos = exepath.rfind("/");
	setenv("EXE_DIR", exepath.substr(0, pos).c_str(), 1);
}

