/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 21:09:14 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/15 17:32:15 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(void) {}

Response::Response(RequestParser &request)
{
	std::string html_file;
	std::string str;
	int			file_exist = 0;
	size_t		pos;
	struct stat	sb;

	//何も指定がない場合はHTML_PATH + /index.htmlが開く
	if (request.get_uri().find_first_not_of("/") != std::string::npos)
		html_file = HTML_PATH + request.get_uri();
	else
		html_file = HTML_PATH + "/index.html";

	//directoryだった場合、autoindex機能を使用するため、stat情報をsbに格納（場所はいずれ変更する予定）
	if (stat(html_file.c_str(), &sb) < 0)
		std::cout << "stat() failed" << std::endl;
//cgi

	//cgiを利用して、autoindex機能を実行。サブプロセスで実行する。
	//ボディがある場合はサブプロセスの標準入力から取得する。
	//responseのボディはサブプロセスの標準出力を受け取る
	if (S_ISDIR(sb.st_mode))
	{
		pid_t	pid;
		int fds_w[2];
		int	fds_r[2];
		char exebuf[CGI_BUF];
		std::string	cgi_file;
		cgi_file = "./GenIndex.py";
		char const	*argv[3] = {"python3", cgi_file.c_str(), NULL};
	
		pipe(fds_w);
		pipe(fds_r);
		pid = fork();
		if (pid < 0)
		{
			std::cerr << "fork_fail" << std::endl;
		}
		if (pid == 0)
		{
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			dup2(fds_w[0], STDIN_FILENO);
			dup2(fds_r[1], STDOUT_FILENO);
			close(fds_r[0]);
			close(fds_r[1]);
			close(fds_w[0]);
			close(fds_w[1]);

			execve("/usr/local/bin/python3", const_cast<char**>(argv), environ);
		}
		else
		{
			int		status;
			int		r;
			close(fds_w[0]);
			close(fds_r[1]);
			int		w;
			if ((w = write(fds_w[1], request.get_body().c_str(), request.get_body().length())) < 0)
				std::cerr << "write():failed" << std::endl;
			close(fds_w[1]);
			while (1)
			{
				r = read(fds_r[0], exebuf, CGI_BUF - 1);
				if (r == 0)
					break;
				else if (r == -1)
					exit(1);
				exebuf[r] = '\0';
				str = exebuf;
				body.append(str);
			}
			wait(&status);
			close(fds_r[0]);
//			std::cout << body << std::endl;
		}	
	}
	//通常のcgi実施のためのelse if 上記autoindex機能のコピペ部分が多いので集約する予定
	else if ((pos = html_file.rfind("/cgi-bin/") != std::string::npos))
	{
		pid_t	pid;
		int fds_w[2];
		int	fds_r[2];
		char exebuf[CGI_BUF];
		std::string	cgi_file;
		cgi_file = HTML_PATH + request.get_script_name();
		char const	*argv[3] = {"python3", cgi_file.c_str(), NULL};
	
		pipe(fds_w);
		pipe(fds_r);
		pid = fork();
		if (pid < 0)
		{
			std::cerr << "fork_fail" << std::endl;
		}
		if (pid == 0)
		{
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			dup2(fds_w[0], STDIN_FILENO);
			dup2(fds_r[1], STDOUT_FILENO);
			close(fds_r[0]);
			close(fds_r[1]);
			close(fds_w[0]);
			close(fds_w[1]);

			execve("/usr/local/bin/python3", const_cast<char**>(argv), environ);
		}
		else
		{
			int		status;
			int		r;

			close(fds_w[0]);
			close(fds_r[1]);
			int		w;
			if ((w = write(fds_w[1], request.get_body().c_str(), request.get_body().length())) < 0)
				std::cerr << "write():failed" << std::endl;
			close(fds_w[1]);
			while (1)
			{
				r = read(fds_r[0], exebuf, CGI_BUF - 1);
				if (r == 0)
					break;
				else if (r == -1)
					exit(1);
				exebuf[r] = '\0';
				str = exebuf;
				body.append(str);
			}
			wait(&status);
			close(fds_r[0]);
//			std::cout << body << std::endl;
		}	
	}
	//cgiを起動させない場合はこっち。ファイルが見つかればそれを開く。見つからなければ404 Not Foundとなるように
	else
	{
		std::ifstream output_file(html_file);
		if ((file_exist = output_file.fail()))
		{
			html_file = HTML_PATH + "/404.html";
			output_file.close();
			output_file.clear();
			output_file.open(html_file);
		}
	
		while (getline(output_file, str))
			body.append(str + "\n");
	}
	
	//Responseをまとめる
    std::ostringstream oss;
	oss << "Content-Length: " << body.length() << "\r\n";
	if (!file_exist)
	{
	    header.append("HTTP/1.1 200 OK\r\n");
	    header.append("Content-Type: text/html; charset=UTF-8\r\n");
	    header.append(oss.str());
	    header.append("Connection: Keep-alive\r\n");
	    header.append("\r\n");
	}
	else
	{
	    header.append("HTTP/1.1 404 Not Found\r\n");
	    header.append("Content-Type: text/html; charset=UTF-8\r\n");
	    header.append(oss.str());
	    header.append("Connection: Keep-alive\r\n");
	    header.append("\r\n");
	}
}

Response::~Response(void) {}

Response::Response(Response const &copy)
{
	*this = copy;
}

Response &Response::operator=(Response const &obj)
{
	if (this != &obj)
	{
		header = obj.header;
		body = obj.body;
	}
	return (*this);
}

std::string Response::get_header(void) {return (header);}
std::string Response::get_body(void) {return (body);}
