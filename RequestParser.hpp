/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 21:50:09 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/16 21:17:36 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include "config.hpp"

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
//		std::vector<std::string>	vec_env;
//		char						**cgi_env;

	public:
		RequestParser(void);
		explicit RequestParser(std::string request_);
		~RequestParser(void);
		RequestParser(RequestParser const &copy);
		RequestParser &operator=(RequestParser const &obj);

		std::string	get_method(void);
		std::string	get_header(void);
		std::string	get_body(void);
		std::string	get_uri(void);
		std::string	get_path_translated(void);
		std::string	get_query_string(void);
		std::string	get_path_info(void);
		std::string	get_script_name(void);
		std::string	get_content_length(void);
		std::string	get_content_type(void);
		std::string	get_field(std::string key);
};

#endif
