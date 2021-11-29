/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 21:09:14 by rtomishi          #+#    #+#             */
/*   Updated: 2021/11/29 22:01:27 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//デフォルトコンストラクタ
Response::Response(void) {}

//コンストラクタ
Response::Response(RequestParser &request)
{
	std::string 		html_file;
	struct stat			eval_directory;
	struct stat			eval_cgi;
	const std::string	EXE_DIR(getenv("EXE_DIR"));

	//何も指定がない(="/"以外の文字が来ない)場合はHTML_PATH + /index.htmlが開く
	if (request.get_uri().find_first_not_of("/") != std::string::npos)
		html_file = EXE_DIR + HTML_PATH + request.get_uri();
	else
		html_file = EXE_DIR + HTML_PATH + "/index.html";

	//CGI起動の場合のため、cgi_file変数を定義。CGIを使用しない場合は使わない
	std::string	cgi_file = EXE_DIR + HTML_PATH + request.get_script_name();

	//html_fileがdirectoryだった場合、autoindex機能を使用するため、stat情報を格納
	stat(html_file.c_str(), &eval_directory);
	//CGI起動の場合、指定しているURIがファイルであるのか評価するためstat情報を格納e
	stat(cgi_file.c_str(), &eval_cgi);

	//ここのif文でbodyを作成
	//cgiを利用して、autoindex機能を実行。サブプロセスで実行する。
	if (S_ISDIR(eval_directory.st_mode))
		status = auto_index(CGI_EXE, AUTOINDEX_CGI);
	//request.get_script_name()がファイルである場合、つまりリクエストされているURIが
	//CGI直下のディレクトリのファイルである場合、CGIを実行する。
	else if (S_ISREG(eval_cgi.st_mode))
		status = cgi_exe(CGI_EXE, cgi_file, request);
	//ファイルが見つかればそれを開く。見つからなければ404 Not Found用のファイルを開く
	else
		status = open_html(html_file);
	
	//以下のswitch文でヘッダーを作成する
    std::ostringstream oss;

	oss << "Content-Length: " << body.length() << "\r\n";
	switch (status)
	{
		case STATUS_OK:
			header_ok(oss);
			break;
		case STATUS_NOT_FOUND:
			header_not_found(oss);
			break;
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
		status = obj.status;
	}
	return (*this);
}

//ゲッター
std::string Response::get_header(void) {return (header);}
std::string Response::get_body(void) {return (body);}
int			Response::get_status(void) {return (status);}

//CGIでautoindex_fileを指定して、サブプロセス実行する
//返り値:実行結果に対するステータスコード
//exe_path:実行するCGIのパス
//auto_index_file:autoindex機能が発現するスクリプトファイル
int		Response::auto_index(std::string const exe_path, std::string const autoindex_file)
{
	pid_t	pid;
	int	fds_r[2];
	char exebuf[CGI_BUF];
	char const	*argv[3] = {exe_path.c_str(), autoindex_file.c_str(), NULL};
	int			ret = STATUS_OK;

	pipe(fds_r);
	pid = fork();
	if (pid < 0)
		std::cerr << "fork_fail" << std::endl;
	if (pid == 0)
	{
		close(STDOUT_FILENO);
		dup2(fds_r[1], STDOUT_FILENO);
		close(fds_r[0]);
		close(fds_r[1]);
		execve(exe_path.c_str(), const_cast<char**>(argv), environ);
	}
	else
	{
		int			status;
		int			r;
		std::string	str;

		close(fds_r[1]);
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
	}
	return (ret);
}

//CGIを起動するための関数
//ボディがある場合はサブプロセスの標準入力から取得する。
//responseのボディはサブプロセスの標準出力を受け取る
//返り値:実行結果に対するステータスコード
//exe_path：実行するCGIのパス
//cgi_file：実行されるCGIの対象ファイル
//request:リクエスト情報が入ったRequestParserクラス
int		Response::cgi_exe(std::string const exe_path, std::string const cgi_file, RequestParser &request)
{
	pid_t	pid;
	int fds_w[2];
	int	fds_r[2];
	char exebuf[CGI_BUF];
	std::string	tmp_str;
	char const	*argv[3] = {exe_path.c_str(), cgi_file.c_str(), NULL};
	int			ret = STATUS_OK;

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

		execve(exe_path.c_str(), const_cast<char**>(argv), environ);
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
			tmp_str = exebuf;
			body.append(tmp_str);
		}
		wait(&status);
		close(fds_r[0]);
//			std::cout << body << std::endl;
	}
	return (ret);
}

//ファイルを開いてレスポンスボディに文字列を格納する。見つからない場合はNot Found処理をする
//返り値:実行結果に対するステータスコード
//html_file:表示させるファイルのパス
int		Response::open_html(std::string html_file)
{
	std::ifstream 	output_file(html_file.c_str());
	std::string		temp_str;
	int				ret = STATUS_OK;
	int				file_exist = 0;

	if ((file_exist = output_file.fail()))
	{
		html_file = HTML_PATH + NOT_FOUND_FILE;
		output_file.close();
		output_file.clear();
		output_file.open(html_file.c_str());
		ret = STATUS_NOT_FOUND;
	}
	while (getline(output_file, temp_str))
		body.append(temp_str + "\n");
	return (ret);
}

//ステータスコード200の場合のヘッダー作成
//oss:開く対象のファイルの出力ストリーム
void	Response::header_ok(std::ostringstream &oss)
{
    header.append("HTTP/1.1 200 OK\r\n");
    header.append("Content-Type: text/html; charset=UTF-8\r\n");
    header.append(oss.str());
    header.append("Connection: Keep-alive\r\n");
    header.append("\r\n");
}

//ステータスコード404の場合のヘッダー作成
//oss:開く対象のファイルの出力ストリーム
void	Response::header_not_found(std::ostringstream &oss)
{
	header.append("HTTP/1.1 404 Not Found\r\n");
	header.append("Content-Type: text/html; charset=UTF-8\r\n");
	header.append(oss.str());
	header.append("Connection: Keep-alive\r\n");
	header.append("\r\n");
}
