/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/13 22:19:12 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/13 17:43:51 by rtomishi         ###   ########.fr       */
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

		std::string	index_search(std::string root, std::vector<std::string> index);
		bool		method_allowed(std::vector<std::string> allowed, RequestParser &request);
		bool		method_limited(std::vector<std::string> limited, RequestParser &request);
		int			cgi_exe(std::string const cgi_file, RequestParser &request, struct stat eval_cgi);
		int			open_html(std::string html_file);
		int			autoindex_c(const char *path, RequestParser &request, bool autoindex);
		int			upload_file(const char *path, RequestParser &request);
		int			delete_file(const char *path);

		void		check_redirect(RequestParser &request);
		void		error_body_set(std::string root_path);
		void		header_set(std::ostringstream &oss);
		void		header_ok(std::ostringstream &oss);
		void		header_not_found(std::ostringstream &oss);

};

#endif
