/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:00:34 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/19 22:34:49 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

RequestParser::RequestParser(void) {}

RequestParser::RequestParser(std::string request_):request(request_), 
													header(""), body("")
{
	std::istringstream	iss(request);
	std::string			str_top;
	std::size_t			pos_head;

	//ヘッダーとボディを分けてメンバ変数に格納
	if ((pos_head = request.find("\r\n\r\n")) != std::string::npos)
	{
		header = request.substr(0, pos_head);
		if (header.length() + 4 != request.length())
			body = request.substr(pos_head + 4);
	}

	//Transfer-Encoding: chunkedの場合、bodyを書き換える
	if (get_field("Transfer-Encoding") == "chunked")
	{
		std::size_t chunk_pos = 0;
		std::size_t	chunk_start = 0;
		std::size_t chunk_end = body.find("\r\n\r\n");
		std::string chunk_body = "";
		std::string	chunk_len_16;
		float		chunk_len;

		//chunkされた文字列を解析して、先頭のバイト数を除いた形で
		//新たにchunk_bodyを形成し、bodyに割り当てる。
		while ((chunk_pos = body.substr(chunk_start).find("\r\n") + chunk_start)
				!= chunk_end)
		{
			chunk_len_16 = "0x" + body.substr(chunk_start, chunk_pos - chunk_start);
			chunk_len = atof(chunk_len_16.c_str());
			chunk_body += body.substr(chunk_start + chunk_len_16.length(), chunk_len);
			chunk_start += chunk_len_16.length() + chunk_len + 2;
		}
		body = chunk_body;
	}


	//メソッドとURIをメンバ変数に格納すると共に、CGI用に環境変数にも設定する
	std::getline(iss, str_top);
	std::istringstream	iss_top(str_top);

	std::getline(iss_top, method, ' ');
	std::getline(iss_top, uri, ' ');

	setenv("REQUEST_METHOD", method.c_str(), 1);
	setenv("REQUEST_URI", uri.c_str(), 1);

//URI checker を後々いれる

	//CGI用の環境変数PATH_TRANSLATED, PATH_INFO...などPATHやパラメータを設定する
	std::size_t script_start;
	std::size_t info_start;
	std::size_t query_start;
	std::string cgi_dir("/cgi-bin/");
	std::string	cgi_para;
	query_string = "";
	path_info = "";
	script_name = "";
	if ((script_start = uri.find("/cgi-bin/")) != std::string::npos)
	{
		if ((query_start = uri.find("?")) != std::string::npos)
		{
			query_string = uri.substr(query_start + 1);
			if ((info_start = uri.substr(script_start + cgi_dir.length()).find("/")) != std::string::npos)
			{
				script_name = uri.substr(0, script_start + cgi_dir.length() + info_start);
				path_info = uri.substr(script_start + cgi_dir.length() + info_start, uri.length() - script_name.length() - query_string.length() - 1);
			}
			else
				script_name = uri.substr(0, uri.length() - query_string.length() - 1);
		}
		else
		{
			if ((info_start = uri.substr(script_start + cgi_dir.length()).find("/")) != std::string::npos)
			{
				script_name = uri.substr(0, script_start + cgi_dir.length() + info_start);
				path_info = uri.substr(script_start + cgi_dir.length() + info_start);
			}
			else
				script_name = uri;
		}
	}
	path_translated = std::string(getenv("PWD")) + path_info;
	setenv("PATH_TRANSLATED", path_translated.c_str(), 1);
	setenv("PATH_INFO", path_info.c_str(), 1);
	setenv("SCRIPT_NAME", script_name.c_str(), 1);
	setenv("QUERY_STRING", query_string.c_str(), 1);

	//Content-Type, Content-Length
	if (get_field("Content-Length") != "")
	{
		content_length = get_field("Content-Length");
		setenv("CONTENT_LENGTH", content_length.c_str(), 1);
	}
	if (get_field("Content-Type") != "")
	{
		content_type = get_field("Content-Type");
		setenv("CONTENT_TYPE", content_type.c_str(), 1);
	}
}

RequestParser::~RequestParser(void) {}

RequestParser::RequestParser(RequestParser const &copy)
{
	*this = copy;
}

RequestParser &RequestParser::operator=(RequestParser const &obj)
{
	if (this != &obj)
	{
		request = obj.request;
		method = obj.method;
		uri = obj.uri;
	}
	return (*this);
}


std::string	RequestParser::get_method(void) {return (this->method);}
std::string	RequestParser::get_header(void) {return (this->header);}
std::string	RequestParser::get_body(void) {return (this->body);}
std::string	RequestParser::get_uri(void) {return (this->uri);}
std::string	RequestParser::get_query_string(void) {return (this->query_string);}
std::string	RequestParser::get_path_translated(void) {return (this->path_translated);}
std::string	RequestParser::get_path_info(void) {return (this->path_info);}
std::string	RequestParser::get_script_name(void) {return (this->script_name);}
std::string	RequestParser::get_content_length(void) {return (this->content_length);}
std::string	RequestParser::get_content_type(void) {return (this->content_type);}
std::string	RequestParser::get_transfer_encoding(void) {return (this->transfer_encoding);}

//ヘッダー情報から値を取り出すためのメンバ関数
std::string	RequestParser::get_field(std::string key)
{
	std::size_t			pos;
	std::istringstream	iss(this->header);
	std::string			line;
	std::string			str;

	while (std::getline(iss, line))
	{
		pos = line.find(key);
		if (pos != std::string::npos && pos == 0)
		{
			str = line.substr(key.length() + 2);
			if (str.find("\r") != std::string::npos)
				return (str.substr(0, str.find("\r")));
			else
				return (str);
		}
	}
	return ("");
}