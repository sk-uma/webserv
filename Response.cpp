/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 21:09:14 by rtomishi          #+#    #+#             */
/*   Updated: 2021/12/17 21:51:41 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//デフォルトコンストラクタ
Response::Response(void) {}

//コンストラクタ
Response::Response(RequestParser &request):content_type("text/html; charset=UTF-8")
{
	std::string 		html_file;
	struct stat			eval_directory;
	struct stat			eval_cgi;
	const std::string	EXE_DIR(getenv("EXE_DIR"));

	//index file設定用。あとでconfigクラスに対応させる
	std::vector<std::string> index;
	index.push_back("index.htm");
	index.push_back("index.html");
	index.push_back("index.nginx-debian.html");

	//method制限テスト用。あとでconfigクラスに対応させる
	std::vector<std::string> allowed;
	std::vector<std::string> limited;
	allowed.push_back("GET");
	allowed.push_back("POST");
	allowed.push_back("DELETE");
	allowed.push_back("HEAD");
	for (std::vector<std::string>::iterator	it = allowed.begin(); it != allowed.end(); it++)
	{
		if (*it != "DELETE")
			limited.push_back(*it);
	}
	
	//リダイレクトのチェック。必要があればuriを書き換える。
	check_redirect(request);

	//何も指定がない(="/"以外の文字が来ない)場合はHTML_PATH + /index.htmlが開く
	if (request.get_uri().find_first_not_of("/") != std::string::npos)
		html_file = EXE_DIR + HTML_PATH + request.get_uri();
	else
		html_file = EXE_DIR + HTML_PATH + index_search(EXE_DIR + HTML_PATH, index);

	//CGI起動の場合のため、cgi_file変数を定義。CGIを使用しない場合は使わない
	std::string	cgi_file = EXE_DIR + HTML_PATH + request.get_script_name();

	//html_fileがdirectoryだった場合、autoindex機能を使用するため、stat情報を格納
	stat(html_file.c_str(), &eval_directory);
	//CGI起動の場合、指定しているURIがファイルであるのか評価するためstat情報を格納e
	stat(cgi_file.c_str(), &eval_cgi);

	//ここのif文でbodyを作成
	if (!method_allowed(allowed, request))
		status = STATUS_METHOD_NOT_ALLOWED;
	else if (!method_limited(limited, request))
		status = STATUS_NOT_IMPLEMENTED;
	else if (request.get_body().length() > CLIENT_MAX_BODY)
		status = STATUS_PAYLOAD_TOO_LARGE;
	else if (request.get_method() == "POST" && request.get_uri() == "/Upload")
		status = upload_file((EXE_DIR + HTML_PATH + UPLOAD_PATH).c_str(), request);
	//methodがDELETEの場合にのみファイルを削除する動作をする
	else if (request.get_method() == "DELETE")
		status = delete_file((EXE_DIR + HTML_PATH + request.get_uri()).c_str());
	//cgiを利用して、autoindex機能を実行。サブプロセスで実行する。
	else if (S_ISDIR(eval_directory.st_mode))
		status = autoindex_c(html_file.c_str(), request, AUTOINDEX);
	//request.get_script_name()がファイルである場合、つまりリクエストされているURIが
	//CGI直下のディレクトリのファイルである場合、CGIを実行する。
	else if (S_ISREG(eval_cgi.st_mode))
		status = cgi_exe(cgi_file, request, eval_cgi);
	//ファイルが見つかればそれを開く。見つからなければ404 Not Found用のファイルを開く
	else
		status = open_html(html_file);
	
	//以下のswitch文でヘッダーを作成する
	if (status >= 400)
		error_body_set(EXE_DIR + HTML_PATH);
		
    std::ostringstream oss;

	oss << "Content-Length: " << body.length() << "\r\n";
//	oss << "Content-Type: " << content_type << "; charset=UTF-8\r\n";
	oss << "Content-Type: " << content_type << "\r\n";
	header_set(oss);
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
std::string Response::get_content_type(void) {return (content_type);}
int			Response::get_status(void) {return (status);}

//indexファイルのvectorからファイル検索をして、ファイルが存在するもののパスを与える
//返り値:見つかったindex_file。見つからなかった場合、空文字
//root:ルートパス文字列
//index:index_fileのリストが格納されたvector
std::string	Response::index_search(std::string root, std::vector<std::string> index)
{
	for (std::vector<std::string>::iterator	it = index.begin(); it != index.end(); it++)
	{
		std::string		path = root + "/" + *it;
		std::ifstream 	file(path.c_str());

		if (file.is_open())
			return ("/" + *it);
	}
	return ("");
}


//許可されているmethod以外が指定されいる場合、falseとする
//返り値:指定外のmethodでfalse
//allowed:許可されているmethodをvectorで格納
//request:リクエスト情報が入ったRequestParserクラス
bool	Response::method_allowed(std::vector<std::string> allowed, RequestParser &request)
{
	for (std::vector<std::string>::iterator	it = allowed.begin(); it != allowed.end(); it++)
	{
		if (*it == request.get_method())
			return (true);
	}
	return (false);
}

//制限されているmethod以外が指定されいる場合、falseとする
//返り値:指定外のmethodでfalse
//allowed:制限されているmethodをvectorで格納
//request:リクエスト情報が入ったRequestParserクラス
bool	Response::method_limited(std::vector<std::string> limited, RequestParser &request)
{
	for (std::vector<std::string>::iterator	it = limited.begin(); it != limited.end(); it++)
	{
		if (*it == request.get_method())
			return (true);
	}
	return (false);
}

//CGIを起動するための関数
//ボディがある場合はサブプロセスの標準入力から取得する。
//responseのボディはサブプロセスの標準出力を受け取る
//返り値:実行結果に対するステータスコード
//exe_path：実行するCGIのパス
//cgi_file：実行されるCGIの対象ファイル
//request:リクエスト情報が入ったRequestParserクラス
int		Response::cgi_exe(std::string const cgi_file, RequestParser &request, struct stat eval_cgi)
{
	//所有者権限がない場合はcgiを起動させない
	if ((eval_cgi.st_mode & S_IXUSR) != S_IXUSR)
		return (STATUS_INTERNAL_SERVER_ERROR);
	pid_t	pid;
	int fds_w[2];
	int	fds_r[2];
	char exebuf[CGI_BUF];
	std::string	tmp_str;
	char const	*argv[2] = {cgi_file.c_str(), NULL};

	pipe(fds_w);
	pipe(fds_r);
	pid = fork();
	if (pid < 0)
		return (STATUS_INTERNAL_SERVER_ERROR);
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
		execve(cgi_file.c_str(), const_cast<char**>(argv), environ);
	}
	else
	{
		int		status;
		int		r;

		close(fds_w[0]);
		close(fds_r[1]);
		int		w;
		if ((w = write(fds_w[1], request.get_body().c_str(), request.get_body().length())) < 0)
			return (STATUS_INTERNAL_SERVER_ERROR);
		close(fds_w[1]);
		while (1)
		{
			r = read(fds_r[0], exebuf, CGI_BUF - 1);
			if (r == 0)
				break;
			else if (r == -1)
				return (STATUS_INTERNAL_SERVER_ERROR);
			exebuf[r] = '\0';
			tmp_str = exebuf;
			body.append(tmp_str);
		}
		wait(&status);
		close(fds_r[0]);
		if (WEXITSTATUS(status))
		{
			body = "";
			return (STATUS_INTERNAL_SERVER_ERROR);
		}
	}
	return (STATUS_OK);
}

//ファイルを開いてレスポンスボディに文字列を格納する。見つからない場合はNot Found処理をする
//返り値:実行結果に対するステータスコード
//html_file:表示させるファイルのパス
int		Response::open_html(std::string html_file)
{
	std::ifstream 	output_file(html_file.c_str(), std::ios::binary);
	std::string		temp_str;
	int				ret = STATUS_OK;
	int				file_exist = 0;

	char	buf;
	FILE	*file;

	if ((file_exist = output_file.fail()))
		return (STATUS_NOT_FOUND);
//	while (getline(output_file, temp_str))
//		body.append(temp_str + "\r\n");
	file = fopen(html_file.c_str(), "rb");
	while (fread(&buf, sizeof(buf), 1, file) > 0)
		body += buf;
	body += "\r\n";
	fclose(file);
	if (status == STATUS_MOVED_PERMANENTLY)
		ret = STATUS_MOVED_PERMANENTLY;
	else
		content_type_set(html_file);
	return (ret);
}

//autoindex with c++
//返り値:実行結果に対応するステータスコード
//path:autoindexレスポンスを返すディレクトリのパス
//request:リクエスト情報をもつRequestParserクラス
int		Response::autoindex_c(const char *path, RequestParser &request, bool autoindex)
{
    DIR 				*dp = opendir(path);
	struct stat 		eval_dir;
	dirent				*entry;
	std::string 		temp_uri = request.get_uri();
    std::ostringstream 	oss;

	if (!autoindex)
		return (STATUS_FORBIDDEN);
	//最後の文字に/があると扱いにくいので除去する
	std::size_t i = 0;
	while (temp_uri[temp_uri.length() - i - 1] == '/')
		i++;
	temp_uri = temp_uri.substr(0, temp_uri.length() - i);

	//リンクの作成。ディレクトリとファイルの判別が着くようにディレクトリの末尾には/を表示させる
	if (dp == NULL)
		return (STATUS_INTERNAL_SERVER_ERROR);
	if ((entry = readdir(dp)) == NULL)
		return (STATUS_INTERNAL_SERVER_ERROR);
	while (entry != NULL)
	{
		if (std::string(entry->d_name) != ".")
		{
			std::string	temp_path = std::string(path) + "/" + entry->d_name;

			stat(temp_path.c_str(), &eval_dir);
			if (std::string(entry->d_name) != ".." && S_ISDIR(eval_dir.st_mode))
				oss << "<li><a href=\"" << temp_uri + "/" + entry->d_name << "\">" << entry->d_name << "/</a></li>\r\n";
			else
				oss << "<li><a href=\"" << temp_uri + "/" + entry->d_name << "\">" << entry->d_name << "</a></li>\r\n";
		}
		entry = readdir(dp);
	}

	body.append("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN>\r\n");
	body.append("<html>\r\n");
	body.append("<head>\r\n");
	body.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n");
	body.append("<title>Directory listing for " + temp_uri + "</title>\r\n");
	body.append("</head>\r\n");
	body.append("<body>\r\n");
	body.append("<h1>Directory listing for " + temp_uri + "</h1>\r\n");
	body.append("<hr>\r\n");
	body.append("<ul>\r\n");
	body.append(oss.str());
	body.append("</ul>\r\n");
	body.append("<hr>\r\n");
	body.append("</body>\r\n");
	body.append("</html>\r\n");

	return (STATUS_OK);
}

//ファイルアップロードをするための関数
//返り値:実行結果に対応するステータスコード
//path:アップロードされるファイルを格納するフォルダパス
//request:リクエスト情報をもつRequestParserクラス
int		Response::upload_file(const char *path, RequestParser &request)
{
	std::istringstream			iss(request.get_body());
	std::string					boundary;
	std::size_t					s_pos = 0;
	std::size_t					len = 0;
	std::vector<std::string>	vstr;
	std::ostringstream			oss;
	
	if (request.get_content_type().find("multipart/form-data") == std::string::npos)
		return (STATUS_BAD_REQUEST);
	getline(iss, boundary);
	boundary = boundary.substr(0, boundary.length() - 1);

	while (1)
	{
		s_pos = boundary.length() + len + s_pos + 2;
		if ((len = request.get_body().substr(s_pos).find(boundary)) == std::string::npos)
			break ;
		vstr.push_back(request.get_body().substr(s_pos, len));
	}
	for (std::vector<std::string>::iterator	it = vstr.begin(); it != vstr.end(); it++)
	{
		std::istringstream	iss_top(*it);
		std::string			top;

		getline(iss_top, top);
		if (top.find("filename=") == std::string::npos)
			return (STATUS_BAD_REQUEST);
	}
	for (std::vector<std::string>::iterator	it = vstr.begin(); it != vstr.end(); it++)
	{
		std::istringstream	iss_top(*it);
		std::string			top;
		std::size_t			start_top;
		std::size_t			filename_len;
		std::string			filename;
		std::string			output;
		
		getline(iss_top, top);
		start_top = top.find("filename=\"") + std::string("filename=\"").length();
		filename_len = top.length() - (start_top + 2);
		filename = top.substr(start_top, filename_len);
		output = (*it).substr((*it).find("\r\n\r\n") + std::string("\r\n\r\n").length());
		output = output.substr(0, output.length() - 2);

		std::ofstream		ofs(std::string(path) + filename, std::ios::binary);
		if (!ofs)
			return(STATUS_INTERNAL_SERVER_ERROR);
		else
		{
			ofs << output;
			oss << "\"" + filename + "\" Upload Successed.\r\n";
		}
	}
	body.append("<html>\r\n");
	body.append("<head>\r\n");
	body.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n");
	body.append("<title>Upload</title>\r\n");
	body.append("</head>\r\n");
	body.append("<body>\r\n");
	body.append(oss.str());
	body.append("</body>\r\n");
	body.append("</html>\r\n");
	return (STATUS_OK);
}

//DELETEメソッド指定時に指定のパスのファイルを消去する
//返り値:実行結果に対するステータスコード
//path:消去するファイルのパス
int		Response::delete_file(const char *path)
{
    std::ostringstream	oss;

	if (remove(path) == 0)
		oss << "Delete Successed.\r\n";
	else
		return (STATUS_BAD_REQUEST);
	body.append("<html>\r\n");
	body.append("<head>\r\n");
	body.append("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\r\n");
	body.append("<title>Delete</title>\r\n");
	body.append("</head>\r\n");
	body.append("<body>\r\n");
	body.append(oss.str());
	body.append("</body>\r\n");
	body.append("</html>\r\n");
	return (STATUS_OK);
}


void	Response::check_redirect(RequestParser &request)
{
	//（修正予定）後でエラーファイルを選択できるようにする
	std::map<std::string, std::string>	redirect_map;
	redirect_map.insert(std::make_pair("/42webserv", "/301.html"));

	if (redirect_map.count(request.get_uri()))
	{
		request.set_uri(redirect_map[request.get_uri()]);
		status = STATUS_MOVED_PERMANENTLY;
	}
}

//エラー用のボディ設定をする
//error_path:エラーファイルが存在するディレクトリ
void	Response::error_body_set(std::string error_path)
{
	//（修正予定）後でエラーファイルを選択できるようにする
	std::map<int, std::string>	ERROR_FILE;
	ERROR_FILE.insert(std::make_pair(STATUS_BAD_REQUEST, "400.html"));
	ERROR_FILE.insert(std::make_pair(STATUS_FORBIDDEN, "403.html"));
	ERROR_FILE.insert(std::make_pair(STATUS_NOT_FOUND, "404.html"));
	ERROR_FILE.insert(std::make_pair(STATUS_METHOD_NOT_ALLOWED, "405.html"));
	ERROR_FILE.insert(std::make_pair(STATUS_PAYLOAD_TOO_LARGE, "413.html"));
	ERROR_FILE.insert(std::make_pair(STATUS_INTERNAL_SERVER_ERROR, "500.html"));
	ERROR_FILE.insert(std::make_pair(STATUS_NOT_IMPLEMENTED, "501.html"));
	
	std::string	path;
	path = error_path + "/" + ERROR_FILE.at(status);

	std::ifstream 	output_file(path.c_str());
	std::string		temp_str;
	int				file_exist = 0;

	if ((file_exist = output_file.fail()))
	{
		status = STATUS_INTERNAL_SERVER_ERROR;
		path = error_path + ERROR_FILE.at(status);
		output_file.close();
		output_file.clear();
		output_file.open(path.c_str());
	}
	while (getline(output_file, temp_str))
		body.append(temp_str + "\r\n");
}

void	Response::header_set(std::ostringstream &oss)
{
	switch (status)
	{
		case STATUS_OK:
    		header.append("HTTP/1.1 200 OK\r\n");
			break;
		case STATUS_MOVED_PERMANENTLY:
    		header.append("HTTP/1.1 301 Moved Permanently\r\n");
    		header.append("Location: https://youtube.com\r\n");
			break;
		case STATUS_BAD_REQUEST:
    		header.append("HTTP/1.1 400 Bad Request\r\n");
			break;
		case STATUS_FORBIDDEN:
    		header.append("HTTP/1.1 403 Forbidden\r\n");
			break;
		case STATUS_NOT_FOUND:
    		header.append("HTTP/1.1 404 Not Found\r\n");
			break;
		case STATUS_METHOD_NOT_ALLOWED:
    		header.append("HTTP/1.1 405 Method Not Allowed\r\n");
			break;
		case STATUS_PAYLOAD_TOO_LARGE:
    		header.append("HTTP/1.1 413 Payload Too Large\r\n");
			break;
		case STATUS_INTERNAL_SERVER_ERROR:
    		header.append("HTTP/1.1 500 Internal Server Error\r\n");
			break;
		case STATUS_NOT_IMPLEMENTED:
    		header.append("HTTP/1.1 501 Not Implemented\r\n");
			break;
	}
//    header.append("Content-Type: text/html; charset=UTF-8\r\n");
    header.append(oss.str());
    header.append("Connection: Keep-alive\r\n");
    header.append("\r\n");
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

void	Response::content_type_set(std::string file_path)
{
	std::ifstream	mime(MIME_FILE.c_str());
	std::string		line;
	std::size_t		pos;
	std::string		extension = file_path.substr(file_path.rfind("/") + 1);
	std::string		type;

	if (mime.fail())
	{
		status = STATUS_INTERNAL_SERVER_ERROR;
		return ;
	}
	if ((pos = extension.rfind(".")) != std::string::npos && pos != 0)
	{
		extension = extension.substr(pos + 1);
		content_type = "text/plain";
		while (getline(mime, line))
		{
			if (line.find(extension + ":") == 0)
			{
				if ((pos = line.find(":")) == std::string::npos)
				{
					status = STATUS_INTERNAL_SERVER_ERROR;
					return ;
				}
				content_type = line.substr(pos + 1);
				break ;
			}
		}
	}
	else
	{
		extension = "";
		content_type = "application/octet-stream";
	}
}
