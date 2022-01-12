/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 22:19:12 by rtomishi          #+#    #+#             */
/*   Updated: 2022/01/12 23:22:45 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "ServerCollection.hpp"
# include "RequestParser.hpp"
# include "Setting.hpp"

class Response
{
	private:
		std::string					header;
		std::string 				body;
		std::string					content_type;
		std::map<int, std::string>	error_map;
		std::map<int, std::string>	code_map;
		int							status;

	public:
		Response(void);
		explicit Response(RequestParser &request, webservconfig::Server &serv);
		~Response(void);
		Response(Response const &copy);
		Response &operator=(Response const &obj);

		std::string	get_header(void);
		std::string get_body(void);
		std::string get_content_type(void);
		std::map<int, std::string> get_error_map(void);
		std::map<int, std::string> get_code_map(void);
		int			get_status(void);

		void		set_error_map(webservconfig::ConfigBase::error_page_type &err_map);
		std::string	index_search(std::string root, std::vector<std::string> index);
		bool		method_limited(webservconfig::Server &serv, RequestParser &request);
		bool		method_allowed(webservconfig::Server &serv, RequestParser &request);
		int			cgi_exe(std::string const cgi_file, RequestParser &request, struct stat eval_cgi);
		int			open_html(std::string html_file);
		int			autoindex_c(const char *path, RequestParser &request, bool autoindex);
		int			upload_file(const char *path, RequestParser &request);
		int			delete_file(const char *path);

		void		error_body_set(webservconfig::ConfigBase::return_type ret_pair);
		void		header_set(std::ostringstream &oss, webservconfig::ConfigBase::return_type ret_pair);
		void		content_type_set(std::string file_path);
		void		status_check(void);
};

#endif
