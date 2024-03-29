/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rtomishi <rtomishi@student.42tokyo.jp      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/14 21:09:14 by rtomishi          #+#    #+#             */
/*   Updated: 2022/02/05 22:10:04 by rtomishi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

//デフォルトコンストラクタ
Response::Response(void) {}

Response::Response(RequestParser &request, webservconfig::Server &serv, int code)
	:content_type("text/html; charset=UTF-8"), status(code)
{
	const std::string	path = request.get_uri();
	webservconfig::ConfigBase::error_page_type	err_map_new = serv.GetErrorPage(path);
	webservconfig::ConfigBase::return_type	ret_pair(std::make_pair(-1, ""));

	set_error_map(err_map_new);
	// std::cout << "status: " << this->status << std::endl;
	status_check();
	// std::cout << "status: " << this->status << std::endl;
	if (status >= 300)
		error_body_set(ret_pair);

	std::ostringstream oss;

	oss << "Server:42Tokyo_webserv";
	oss << "\r\n";
	oss << "Content-Length: " << body.length() << "\r\n";
	oss << "Content-Type: " << content_type << "\r\n";
	header_set(oss, ret_pair);
}

//コンストラクタ
Response::Response(RequestParser &request, webservconfig::Server &serv)
	:content_type("text/html; charset=UTF-8"), status(0)
{
	std::string 		html_file;
	struct stat			eval_directory;
	struct stat			eval_cgi;
	const std::string	path = request.get_uri();
	const std::string	EXE_DIR(getenv("EXE_DIR"));
	const std::string	HTML_PATH = serv.GetRoot(path);
	const std::vector<std::string>	index = serv.GetIndex(path);
	const unsigned long	CLIENT_MAX_BODY = serv.GetClientMaxBodySize(path);
	const std::string	UPLOAD_PATH = serv.GetUploadPath(path);
	const bool			AUTOINDEX = serv.GetAutoIndex(path);
	webservconfig::ConfigBase::error_page_type	err_map_new = serv.GetErrorPage(path);
	webservconfig::ConfigBase::return_type	ret_pair = serv.GetReturn(path);
	webservconfig::ConfigBase::server_name_list_type	name = serv.GetServerName();

	// cgi env
	set_cgi_env(request, serv);

	//エラーページを設定
	set_error_map(err_map_new);

	//何も指定がない(="/"以外の文字が来ない)場合はHTML_PATH + /index.htmlが開く
	if (request.get_uri().find_first_not_of("/") != std::string::npos)
		html_file = EXE_DIR + HTML_PATH + request.get_uri();
	else
		html_file = EXE_DIR + HTML_PATH + index_search(EXE_DIR + HTML_PATH, index);

	//urlに日本語がある場合、ブラウザでurlがエンコードされるのででコード
	html_file = urlDecode(html_file);
	//CGI起動の場合のため、cgi_file変数を定義。CGIを使用しない場合は使わない
	std::string	cgi_file = EXE_DIR + HTML_PATH + this->script_name;

	//urlに日本語がある場合、ブラウザでurlがエンコードされるのででコード(CGI用)
	cgi_file = urlDecode(cgi_file);

	//html_fileがdirectoryだった場合、autoindex機能を使用するため、stat情報を格納
	stat(html_file.c_str(), &eval_directory);

	//urlがdirctory、autoindex=off, directory内にindexファイルがある場合、indexファイルを開くようにする
	if ((S_ISDIR(eval_directory.st_mode)) && !AUTOINDEX &&
			index_search(html_file, index) != "")
	{
		html_file = html_file + index_search(EXE_DIR + HTML_PATH, index);
		stat(html_file.c_str(), &eval_directory);
	}
	//CGI起動の場合、指定しているURIがファイルであるのか評価するためstat情報を格納e
	stat(cgi_file.c_str(), &eval_cgi);

	//ここのif文でbodyを作成
	if (ret_pair.first > 0)
	{
		status = ret_pair.first;
		if (ret_pair.first < 300)
		{
			if (ret_pair.second != "")
			{
				body = ret_pair.second + "\r\n";
				content_type = "application/octet-stream";
			}
		}
		// std::cerr << "[*] " << "cleate body" << std::endl;
		write(2, "[*] create body\n", 17);
	}
	else if (!method_limited(serv, request)) {
		status = STATUS_NOT_IMPLEMENTED;
		// std::cerr << "[*] " << "not implemented" << std::endl;
		write(2, "[*] not implemented\n", 21);
	} else if (!method_allowed(serv, request)) {
		status = STATUS_METHOD_NOT_ALLOWED;
		// std::cerr << "[*] " << "method not allowed" << std::endl;
		write(2, "[*] method not allowed\n", 24);
	} else if (request.get_body().length() > CLIENT_MAX_BODY) {
		status = STATUS_PAYLOAD_TOO_LARGE;
		// std::cerr << "[*] " << "payload too large" << std::endl;
		write(2, "[*] payload too large\n", 23);
	} else if (request.get_method() == "POST" && request.get_uri() == "/Upload") {
		status = upload_file((EXE_DIR + UPLOAD_PATH + "/").c_str(), request);
		// std::cerr << "[*] " << "requests POST method" << std::endl;
		write(2, "[*] requests POST method\n", 26);
	//methodがDELETEの場合にのみファイルを削除する動作をする
	} else if (request.get_method() == "DELETE") {
		status = delete_file((EXE_DIR + HTML_PATH + request.get_uri()).c_str());
		// std::cerr << "[*] " << "requests DELETE method" << std::endl;
		write(2, "[*] requests DELETE method\n", 28);
	//autoindex機能を実行。
	} else if (S_ISDIR(eval_directory.st_mode)) {
		status = autoindex_c(html_file.c_str(), request, AUTOINDEX);
		// std::cerr << "[*] " << "requests autoindex" << std::endl;
		write(2, "[*] requests autoindex\n", 24);
	//request.get_script_name()がファイルである場合、つまりリクエストされているURIが
	//CGI直下のディレクトリのファイルである場合、CGIを実行する。
	} else if (S_ISREG(eval_cgi.st_mode)) {
		status = cgi_exe(cgi_file, request, eval_cgi);
		// std::cerr << "[*] " << "requests CGI" << std::endl;
		write(2, "[*] requests CGI\n", 18);
	//ファイルが見つかればそれを開く。見つからなければ404 Not Found用のファイルを開く
	} else {
		status = open_html(html_file);
		// std::cerr << "[*] " << "open html" << std::endl;
		write(2, "[*] open html\n", 15);
	}
	
	//実装していないコードは403 Forbiddenで処理する
	status_check();

	//以下のswitch文でヘッダーを作成する
	if (status >= 300)
		error_body_set(ret_pair);

	std::ostringstream oss;

	oss << "Server:42Tokyo_webserv\r\n";
	oss << "Content-Length: " << body.length() << "\r\n";
	oss << "Content-Type: " << content_type << "\r\n";
	header_set(oss, ret_pair);
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
		content_type = obj.content_type;
		error_map = obj.error_map;
		status = obj.status;
		path_translated = obj.path_translated;
		query_string = obj.query_string;
		path_info = obj.path_info;
		script_name = obj.script_name;
	}
	return (*this);
}

//ゲッター
std::string 				Response::get_header(void) {return (header);}
std::string 				Response::get_body(void) {return (body);}
std::string 				Response::get_content_type(void) {return (content_type);}
std::map<int, std::string> 	Response::get_error_map(void) {return (error_map);}
std::map<int, std::string> 	Response::get_code_map(void) {return (code_map);}
int							Response::get_status(void) {return (status);}

//エラーページの書き換えが必要な場合、書き換える
void	Response::set_error_map(webservconfig::ConfigBase::error_page_type &err_map_new)
{
	const std::string	EXE_DIR(getenv("EXE_DIR"));
	std::ifstream		code((EXE_DIR + CODE_FILE).c_str());
	std::string			line;

	while (getline(code, line))
	{
		std::stringstream	ss(line);
		std::string			c, s;

		getline(ss, c, ':');
		error_map.insert(std::make_pair(atoi(c.c_str()), EXE_DIR + ERROR_PAGE_DIRECTORY + c + ".html"));
		getline(ss, s, ':');
		code_map.insert(std::make_pair(atoi(c.c_str()), s));
	}
	for (webservconfig::ConfigBase::error_page_type::iterator	it = err_map_new.begin(); it != err_map_new.end(); it++)
		error_map[(*it).first] = EXE_DIR + (*it).second;
}

//indexファイルのvectorからファイル検索をして、ファイルが存在するもののパスを与える
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
bool	Response::method_allowed(webservconfig::Server &serv, RequestParser &request)
{
	webservconfig::ConfigBase::limit_except_type allowed = serv.GetLimitExceptByDenyAll(request.get_uri());
	if (allowed[request.get_method()])
		return (true);
	return (false);
}

//制限されているmethod以外が指定されいる場合、falseとする
bool	Response::method_limited(webservconfig::Server &serv, RequestParser &request)
{
	webservconfig::ConfigBase::limit_except_type limited = serv.GetLimitExceptByDenyAll(request.get_uri());
	for (webservconfig::ConfigBase::limit_except_type::iterator	it = limited.begin(); it != limited.end(); it++)
	{
		if ((*it).first == request.get_method())
			return (true);
	}
	return (false);
}

//CGIを起動するための関数
//ボディがある場合はサブプロセスの標準入力から取得する。
//responseのボディはサブプロセスの標準出力を受け取る
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
	content_type_set(html_file);
	if (status == STATUS_INTERNAL_SERVER_ERROR)
		return (STATUS_INTERNAL_SERVER_ERROR);
	file = fopen(html_file.c_str(), "rb");
	while (fread(&buf, sizeof(buf), 1, file) > 0)
		body += buf;
	body += "\r\n";
	fclose(file);
	return (ret);
}

//autoindex with c++
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
	body.append("<title>Directory listing for " + urlDecode(temp_uri) + "</title>\r\n");
	body.append("</head>\r\n");
	body.append("<body>\r\n");
	body.append("<h1>Directory listing for " + urlDecode(temp_uri) + "</h1>\r\n");
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
int		Response::upload_file(const char *path, RequestParser &request)
{
	std::istringstream			iss(request.get_body());
	std::string					boundary;
	std::size_t					s_pos = 0;
	std::size_t					len = 0;
	std::vector<std::string>	vstr;
	std::ostringstream			oss;
	
	if (request.get_content_type().find("multipart/form-data") == std::string::npos || request.get_body() == "")
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

		std::ofstream		ofs((std::string(path) + filename).c_str(), std::ios::binary);
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

//エラー用のボディ設定をする
void	Response::error_body_set(webservconfig::ConfigBase::return_type ret_pair)
{
	std::string	path;

	if (!(status == 301 || status == 302 || status == 303
				|| status == 307 || status == 308) && ret_pair.second != "")
	{
		body = ret_pair.second + "\r\n";
		content_type = "application/octet-stream";
		return ;
	}
	if (error_map.count(status))
		path = error_map.at(status);
	std::ifstream 	output_file(path.c_str());
	std::string		temp_str;
	int				file_exist = 0;

	if ((file_exist = output_file.fail()))
	{
		status = STATUS_INTERNAL_SERVER_ERROR;
		path = error_map.at(status);
		output_file.close();
		output_file.clear();
		output_file.open(path.c_str());
	}
	while (getline(output_file, temp_str))
		body.append(temp_str + "\r\n");
}

void	Response::header_set(std::ostringstream &oss, webservconfig::ConfigBase::return_type ret_pair)
{
	std::stringstream ss;

	if (error_map.count(status))
	{
		ss << "HTTP/1.1 " << status << " " + code_map[status] << "\r\n";
		if (status == 301 || status == 302 || status == 303
				|| status == 307 || status == 308)
			ss << "Location: " + ret_pair.second + "\r\n";
	}
	else
		ss << "HTTP/1.1 " << status << "\r\n";
	header.append(ss.str());
    header.append(oss.str());
    header.append("Connection: Keep-alive\r\n");
    header.append("\r\n");
}

void	Response::content_type_set(std::string file_path)
{
	const std::string	EXE_DIR(getenv("EXE_DIR"));
	std::ifstream		mime((EXE_DIR + MIME_FILE).c_str());
	std::string			line;
	std::size_t			pos;
	std::string			extension = file_path.substr(file_path.rfind("/") + 1);
	std::string			type;

	if (mime.fail())
	{
		status = STATUS_INTERNAL_SERVER_ERROR;
		return ;
	}
	if ((pos = extension.rfind(".")) != std::string::npos && pos != 0)
	{
		extension = extension.substr(pos + 1);
		content_type = "text/plain; charset=UTF-8";
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

void	Response::status_check(void)
{
	if (!(status == STATUS_OK ||
			status == STATUS_MOVED_PERMANENTLY ||
			status == STATUS_FOUND ||
			status == STATUS_SEE_OTHER ||
			status == STATUS_TEMPORARY_REDIRECT ||
			status == STATUS_PERMANENT_REDIRECT ||
			status == STATUS_BAD_REQUEST ||
			status == STATUS_FORBIDDEN ||
			status == STATUS_NOT_FOUND ||
			status == STATUS_METHOD_NOT_ALLOWED ||
			status == STATUS_PAYLOAD_TOO_LARGE ||
			status == STATUS_INTERNAL_SERVER_ERROR ||
			status == STATUS_NOT_IMPLEMENTED))
		status = STATUS_FORBIDDEN;
}

void		Response::set_env_for_header_(const RequestParser &rp, const std::string &field, const std::string &env_name)
{
	std::string res;

	if ((res = rp.get_field(field.c_str())) != "") {
		setenv(env_name.c_str(), res.c_str(), 1);
	} else {
		unsetenv(env_name.c_str());
	}
}

//CGI用の環境変数を設定する
void		Response::set_cgi_env(const RequestParser &rp, const webservconfig::Server &serv)
{
	std::size_t 		info_start;
	std::size_t 		query_start;
	std::size_t			i = 0;
	std::string uri = rp.get_uri();
	const std::string	root = serv.GetRoot(uri);
	
	query_string = "";
	path_info = "";
	script_name = "";
	//先頭に複数/があった場合は除去する
	while (uri[i] == '/')
		i++;
	if (i != 0)
		i--;
	uri = uri.substr(i);


	if ((query_start = uri.find("?")) != std::string::npos)
		query_string = uri.substr(query_start + 1);
	uri = uri.substr(0, query_start);
	webservconfig::ConfigBase::extension_list_type ex = serv.GetCgiExtension(uri);

	for (size_t	c = 0; c < ex.size(); c++)
	{
		if (uri.rfind(ex[c]) == (uri.length() - ex[c].length()))
		{
			script_name = uri;
			break ;
		}
		else if (uri.rfind(ex[c] + "/") != std::string::npos)
		{
			info_start = uri.rfind(ex[c] + "/") + ex[c].length();
			i = info_start;
			while (uri[i] == '/')
				i++;
			if (i != info_start)
				i--;
			path_info = uri.substr(i);
			script_name = uri.substr(0, info_start);
			break ;
		}
	}
	path_translated = root + (root[root.length() - 1] != '/' ? "": "/") + path_info;
	setenv("X_REQUEST_URI", rp.get_uri().c_str(), 1);

	setenv("REQUEST_METHOD", rp.get_method().c_str(), 1);
	// this->set_env_for_header_(rp, "Authorization", "AUTH_TYPE");
	this->set_env_for_header_(rp, "Content-Length", "CONTENT_LENGTH");
	this->set_env_for_header_(rp, "Content-Type", "CONTENT_TYPE");
	setenv("SERVER_SOFTWARE", "webserv", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("PATH_INFO", path_info.c_str(), 1);
	setenv("PATH_TRANSLATED", path_translated.c_str(), 1);
	setenv("QUERY_STRING", query_string.c_str(), 1);
	setenv("SCRIPT_NAME", script_name.c_str(), 1);
	if (serv.GetServerName().size() != 0) {
		setenv("SERVER_NAME", serv.GetServerName().at(0).c_str(), 1);
	} else {
		setenv("SERVER_NAME", "", 1);
	}
}
