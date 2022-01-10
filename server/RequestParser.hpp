/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 21:50:09 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/06 21:29:23 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include "Setting.hpp"

class RequestParser
{
	private:
		std::string					request;
		std::string					header;
		std::string					body;
		std::string					method;
		std::string					uri;
		std::string					path_translated;
		std::string					query_string;
		std::string					path_info;
		std::string					script_name;
		std::string					content_length;
		std::string					content_type;
		std::string					transfer_encoding;

	public:
		RequestParser(void);
		explicit RequestParser(std::string request_, webservconfig::Server &serv);
		~RequestParser(void);
		RequestParser(RequestParser const &copy);
		RequestParser &operator=(RequestParser const &obj);

		std::string	get_method(void) const;
		std::string	get_header(void) const;
		std::string	get_body(void) const;
		std::string	get_uri(void) const;
		std::string	get_path_translated(void) const;
		std::string	get_query_string(void) const;
		std::string	get_path_info(void) const;
		std::string	get_script_name(void) const;
		std::string	get_content_length(void) const;
		std::string	get_content_type(void) const;
		std::string	get_transfer_encoding(void) const;
		std::string	get_field(std::string key) const;

		void		set_uri(std::string uri_);

		void		header_split(void);
		void		chunked_split(void);
		void		set_method_and_uri(void);
		void		set_cgi_env(webservconfig::Server &serv);
};

#endif
