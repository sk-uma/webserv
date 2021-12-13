/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:26:18 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/13 17:45:21 by rtomishi         ###   ########.fr       */
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
# include <map>

const int			BUF_SIZE = 1024;
const int			CGI_BUF = 100;
const unsigned long	CLIENT_MAX_BODY = 10000000;
const std::string	CGI_PATH = "/cgi-bin/";
const std::string	UPLOAD_PATH = "/upload_file/";
const std::string	AUTOINDEX_CGI = "./GenIndex.py";
const std::string	CGI_EXE = "/usr/local/bin/python3";
const std::string	NOT_FOUND_FILE = "/404.html";
const int			MAX_SESSION = 10;
const std::string	HTML_PATH = "/www";
extern char			**environ;

const bool	AUTOINDEX = true;

//レスポンスステータス
const int	STATUS_OK = 200;
const int	STATUS_MOVED_PERMANENTLY = 301;
const int	STATUS_BAD_REQUEST = 400;
const int	STATUS_FORBIDDEN = 403;
const int	STATUS_NOT_FOUND = 404;
const int	STATUS_METHOD_NOT_ALLOWED = 405;
const int	STATUS_PAYLOAD_TOO_LARGE = 413;
const int	STATUS_INTERNAL_SERVER_ERROR = 500;
const int	STATUS_NOT_IMPLEMENTED = 501;

void				setenv_exedir(char **argv);

#endif