/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:26:18 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/30 17:28:10 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <iostream>
# include <cstdlib>
# include <cstring>
# include <unistd.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/types.h>
# include <vector>
# include <fstream>
# include <iterator>
# include <sstream>
# include <netdb.h>
# include <fcntl.h>
# include <sys/select.h>
# include <sstream>
# include <sys/stat.h>
# include <sys/wait.h>
# include <dirent.h>

const int			BUF_SIZE = 1024;
const int			CGI_BUF = 100;
const std::string	CGI_PATH = "/cgi-bin/";
const std::string	AUTOINDEX_CGI = "./GenIndex.py";
const std::string	CGI_EXE = "/usr/local/bin/python3";
const std::string	NOT_FOUND_FILE = "/404.html";
const int			MAX_SESSION = 10;
const std::string	HTML_PATH = "/www";
extern char			**environ;

//レスポンスステータス
const int	STATUS_OK = 200;
const int	STATUS_NOT_FOUND = 404;

void				setenv_exedir(char **argv);

#endif