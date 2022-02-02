/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/12 22:00:34 by rtomishi          #+#    #+#             */
/*   Updated: 2022/02/02 11:10:10 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"

//デフォルトコンストラクタ
RequestParser::RequestParser(void) {}

//コンストラクタ
RequestParser::RequestParser(std::string request_)
	:request(request_),	header(""), body(""), method(""), uri(""),
	content_length(""),	content_type(""), transfer_encoding(""), http_version(""),
	error_status(200)
{
	// std::cerr << "in constructor" << std::endl << std::flush;
	// write(2, "in constructor\n", 16);
	//ヘッダーとボディを分けてメンバ変数に格納する
	header_split();

	//Transfer-Encoding: chunkedの場合、bodyを書き換える
	if (get_field("Transfer-Encoding") == "chunked")
		chunked_split();

	//メソッドとURIをメンバ変数に格納すると共に、環境変数にも設定する
	set_method_and_uri();

	//ボディがある場合、Content-Type, Content-Lengthを設定
	content_length = get_field("Content-Length");
	content_type = get_field("Content-Type");
	if (get_field("Host") == "") {
		this->error_status = 400;
		// std::cout << "Host: not found" << std::endl;
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

		header = obj.header;
		body = obj.body;
		content_length = obj.content_length;
		content_type = obj.content_type;
		transfer_encoding = obj.transfer_encoding;
		http_version = obj.http_version;
		error_status = obj.error_status;
	}
	return (*this);
}

std::string	RequestParser::get_method(void) const {return (this->method);}
std::string	RequestParser::get_header(void) const {return (this->header);}
std::string	RequestParser::get_body(void) const {return (this->body);}
std::string	RequestParser::get_uri(void) const {return (this->uri);}
std::string	RequestParser::get_content_length(void) const {return (this->content_length);}
std::string	RequestParser::get_content_type(void) const {return (this->content_type);}
std::string	RequestParser::get_transfer_encoding(void) const {return (this->transfer_encoding);}
int	RequestParser::get_error_status(void) const {return (this->error_status);}

//ヘッダー情報から値を取り出すためのメンバ関数
std::string	RequestParser::get_field(std::string key) const
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
	std::getline(iss_top, http_version);
	// std::cout << "in set" << std::endl;
	// std::cerr << "[" << http_version << "]" << std::endl;
	// std::cerr << "chr: " << std::strchr(http_version.c_str(), ' ') << std::endl;
	// char *res = std::strchr(http_version.c_str(), ' ');
	int res = std::strcmp(http_version.c_str(), "HTTP/1.1\r");
	// for (int i = 0; i < (int)std::strlen(http_version.c_str()); i++) {
	// 	std::cout << i << ": " << (int)http_version.c_str()[i] << std::endl;
	// }
	// std::cout << "res: " << std::strlen(http_version.c_str()) << std::endl;
	if (res) {
		// write(2, "res: 0\nmethod: [", 17);
		// write(2, method.c_str(), std::strlen(method.c_str()));
		// write(2, "]\nuri: [", 9);
		// write(2, uri.c_str(), std::strlen(uri.c_str()));
		// write(2, "]\nhttp_version: [", 18);
		// write(2, http_version.c_str(), std::strlen(http_version.c_str()));
		// write(2, "]", 2);
		// write(2, "\n", 2);
		if (!std::strcmp(http_version.substr(0, 5).c_str(), "HTTP/")) {
			this->error_status = 505;
		} else {
			this->error_status = 400;
		}
	}
	// setenv("X_REQUEST_METHOD", method.c_str(), 1);
	// setenv("X_REQUEST_URI", uri.c_str(), 1);
}

