/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:00:34 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/15 21:06:30 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

//デフォルトコンストラクタ
RequestParser::RequestParser(void) {}

//コンストラクタ
RequestParser::RequestParser(std::string request_):request(request_),
								header(""), body(""), method(""), uri(""),
								path_translated(""), query_string(""),
								path_info(""), script_name(""), content_length(""),
								content_type(""), transfer_encoding("")
{
	//ヘッダーとボディを分けてメンバ変数に格納する
	header_split();

	//Transfer-Encoding: chunkedの場合、bodyを書き換える
	if (get_field("Transfer-Encoding") == "chunked")
		chunked_split();

	//メソッドとURIをメンバ変数に格納すると共に、環境変数にも設定する
	set_method_and_uri();

	//CGI用の環境変数PATH_TRANSLATED, PATH_INFO...などを設定する
	set_cgi_env();

	//ボディがある場合、Content-Type, Content-Lengthを設定
	content_length = get_field("Content-Length");
	content_type = get_field("Content-Type");
	setenv("CONTENT_LENGTH", content_length.c_str(), 1);
	setenv("CONTENT_TYPE", content_type.c_str(), 1);	
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
//返り値:ヘッダー項目の値。該当する項目がない場合は空文字列で返す
//key:ヘッダー項目
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

//セッター uri
void		RequestParser::set_uri(std::string uri_)
{
	uri = uri_;
}

//ボデイがある場合はリクエストの文字列からヘッダーとボディを分離する
void		RequestParser::header_split(void)
{
	std::size_t			pos_head;

	//ヘッダーとボディを分けてメンバ変数に格納
	pos_head = request.find("\r\n\r\n");
	header = request.substr(0, pos_head);
	if (header.length() + 4 != request.length())
		body = request.substr(pos_head + 4);
}

//チャンク形式のボディ送信の場合、ボディをまとめる操作をする
void		RequestParser::chunked_split(void)
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

//メソッドとURIをメンバ変数と環境変数に両方設定する
void		RequestParser::set_method_and_uri(void)
{
	std::istringstream	iss(header);
	std::string			str_top;

	std::getline(iss, str_top);
	std::istringstream	iss_top(str_top);
	std::getline(iss_top, method, ' ');
	std::getline(iss_top, uri, ' ');
	setenv("REQUEST_METHOD", method.c_str(), 1);
	setenv("REQUEST_URI", uri.c_str(), 1);
}

//CGI用の環境変数を設定する
void		RequestParser::set_cgi_env(void)
{
	std::size_t script_start;
	std::size_t info_start;
	std::size_t query_start;
//	std::string cgi_dir(CGI_PATH);
//	std::string	cgi_para;

	query_string = "";
	path_info = "";
	script_name = "";

	std::size_t	i = 0;
	
	//先頭に複数/があった場合は除去する
	while (uri[i] == '/')
		i++;
	if (i != 0)
		i--;
	uri = uri.substr(i);

	if ((query_start = uri.find("?")) != std::string::npos)
		query_string = uri.substr(query_start + 1);
	uri = uri.substr(0, query_start);
	if (uri.find(CGI_PATH) == 0 &&
			(script_start = uri.substr(CGI_PATH.length()).find_first_not_of("/")) != std::string::npos)
	{
		if ((info_start = uri.substr(CGI_PATH.length() + script_start).find("/")) != std::string::npos)
		{
			i = CGI_PATH.length() + script_start + info_start;
			while (uri[i] == '/')
				i++;
			if (i != CGI_PATH.length() + script_start + info_start)
				i--;
			path_info = uri.substr(i);
		}
		script_name = (path_info == "" ? uri : uri.substr(0, CGI_PATH.length() + script_start + info_start));
	}
	//PATH_TRANSLATEDの指定。環境変数EXE_DIRが相対パス/絶対パスの場合で指定方法を分ける
	if (std::string(getenv("EXE_DIR")).find("/") == 0)
		path_translated = std::string(getenv("EXE_DIR")) + path_info;
	else
		path_translated = std::string(getenv("PWD")) + "/" + std::string(getenv("EXE_DIR")) + path_info;
	setenv("PATH_TRANSLATED", path_translated.c_str(), 1);
	setenv("PATH_INFO", path_info.c_str(), 1);
	setenv("SCRIPT_NAME", script_name.c_str(), 1);
	setenv("QUERY_STRING", query_string.c_str(), 1);
}

