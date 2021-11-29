/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 22:19:12 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/28 22:49:35 by rtomishi         ###   ########.fr       */
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
		int			auto_index(std::string const exe_path, std::string const autoindex_file);
		int			cgi_exe(std::string const exe_path, std::string const cgi_file, RequestParser &request);
		int			open_html(std::string const html_file);

		void		header_ok(std::ostringstream &oss);
		void		header_not_found(std::ostringstream &oss);
};

#endif
