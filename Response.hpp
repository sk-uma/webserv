/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 22:19:12 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/06 13:17:58 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "config.hpp"
# include "RequestParser.hpp"

class Response
{
	private:
		std::string	header;
		std::string body;
		int			status;

	public:
		Response(void);
		explicit Response(RequestParser &request);
		~Response(void);
		Response(Response const &copy);
		Response &operator=(Response const &obj);

		std::string	get_header(void);
		std::string get_body(void);
		int			get_status(void);
		int			auto_index(std::string const autoindex_file);
		int			cgi_exe(std::string const cgi_file, RequestParser &request);
		int			open_html(std::string html_file, std::string exe_path);
		int			autoindex_c(const char *path, RequestParser &request);
		int			upload_file(const char *path, RequestParser &request);
		int			delete_file(const char *path);

		void		header_ok(std::ostringstream &oss);
		void		header_not_found(std::ostringstream &oss);

};

#endif
