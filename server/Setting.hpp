/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Setting.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:26:18 by rtomishi          #+#    #+#             */
/*   Updated: 2022/02/05 22:16:29 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	SETTING_HPP
# define SETTING_HPP

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
# include <signal.h>
# include <cerrno>
# include <limits.h>
# include <ctype.h>

const unsigned long	REQUEST_SIZE = 10000000;
const int			CGI_BUF = 100;
const unsigned long	RESPONSE_BUFFER_SIZE = 10000000;
const std::string	MIME_FILE = "/server/mime";
const std::string	CODE_FILE = "/server/status_code";
const std::string	ERROR_PAGE_DIRECTORY = "/server/error_file/";
const int			MAX_SESSION = 100;
extern char			**environ;
const std::string	CONTINUE_RESPONSE = "HTTP/1.1 100 Continue\r\n\r\n";

//レスポンスステータス
const int	STATUS_CONTINUE = 100;
const int	STATUS_OK = 200;
const int	STATUS_MOVED_PERMANENTLY = 301;
const int	STATUS_FOUND = 302;
const int	STATUS_SEE_OTHER = 303;
const int	STATUS_TEMPORARY_REDIRECT = 307;
const int	STATUS_PERMANENT_REDIRECT = 308;
const int	STATUS_BAD_REQUEST = 400;
const int	STATUS_FORBIDDEN = 403;
const int	STATUS_NOT_FOUND = 404;
const int	STATUS_METHOD_NOT_ALLOWED = 405;
const int	STATUS_PAYLOAD_TOO_LARGE = 413;
const int	STATUS_INTERNAL_SERVER_ERROR = 500;
const int	STATUS_NOT_IMPLEMENTED = 501;

#endif