/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 21:26:18 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/02 22:24:57 by rtomishi         ###   ########.fr       */
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

const int			BUF_SIZE = 1024;
const int			CGI_BUF = 100;
const std::string	HTTP1_PORT = "5000";
const int			HTTP_VERSION = 1;
const int			MAX_SESSION = 10;
const std::string	HTML_PATH = "www";
extern char			**environ;

std::string	header_parser(std::string header, std::string key);

#endif