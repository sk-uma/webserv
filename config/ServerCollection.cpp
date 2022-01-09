#include "ServerCollection.hpp"

webservconfig::ServerCollection::ServerCollection():
  ConfigBase(),
  file_path_(),
  server_()
{ }

/**
 * Config(std::string path)
 * 
 * 引数
 *   path: configファイルのパス
 * 説明
 *   configファイルの内容を解析しメンバ変数に代入する
 *   内容に誤りがある場合には例外を投げる
 *   ディレクティブを解析したのちにServerディレクティブのパースに移る
 */

webservconfig::ServerCollection::ServerCollection(std::string path):
  ConfigBase(),
  file_path_(path),
  server_()
{
  std::ifstream input_file(this->file_path_.c_str());
  std::string line;

  if (!input_file.is_open()) {
    throw std::invalid_argument("can't open the file");
  }

  int i = 1;
  while (std::getline(input_file, line)) {
    if (line.size() != 0 && *(line.end() - 1) == ';') {
      line = std::string(line.begin(), line.end() - 1);
    }
    // std::cout << i++ << ": " << line << std::endl;
    std::vector<std::string> rtv = webservconfig::SplitLine(line);
    int len = rtv.size();
    if (len == 0) {
      continue ;
    }
    if (webservconfig::CountFrontSpace(line) != 0) {
      throw std::runtime_error("invalud indent");
    }
    if (len >= 2 && rtv[0] == "server" && len == 2 && rtv[1] == "{") {
      InitServer(rtv, input_file);
    } else if (rtv[0] == "index") {
      InitIndex(rtv);
    } else if (rtv[0] == "error_page") {
      InitErrorPage(rtv);
    } else if (rtv[0] == "autoindex") {
      InitAutoindex(rtv);
    } else if (rtv[0] == "client_max_body_size") {
      InitClientMaxBodySize(rtv);
    } else if (rtv[0] == "root") {
      InitRoot(rtv);
    } else if (rtv[0] == "cgi_extension") {
      InitCgiExtension(rtv);
    } else {
      throw std::runtime_error(std::string("not allowed directive \"") + rtv[0] + std::string("\""));
    }
  }
  for (std::vector<Server>::iterator iter = this->server_.begin(); iter != this->server_.end(); iter++) {
    iter->SetIndex(this->index_);
    iter->SetErrorPage(this->error_page_);
    iter->SetAutoIndex(this->autoindex_);
    iter->SetClientMaxBodySize(this->client_max_body_size_);
    iter->SetRoot(this->root_);
    iter->SetCgiExtension(this->cgi_extension_);
    iter->ParseServerBlock();
  }
  (void)i;
}

webservconfig::ServerCollection::~ServerCollection()
{ }

webservconfig::ServerCollection::ServerCollection(const ServerCollection &other)
{
  *this = other;
}

const webservconfig::ServerCollection &webservconfig::ServerCollection::operator=(const ServerCollection &rhs)
{
  if (this != &rhs) {
    ConfigBase::operator=(rhs);
    this->file_path_ = rhs.file_path_;
    this->server_ = rhs.server_;
  }
  return (*this);
}

void webservconfig::ServerCollection::InitServer(std::vector<std::string> line, std::ifstream &input_file)
{
  std::string block, buf;

  block = "";
  while (std::getline(input_file, buf)) {
    if (buf == "}") {
      break;
    } else {
      // std::cout << "in server: " << buf << std::endl;
      block += buf + "\n";
    }
  }
  webservconfig::Server server(block);
  this->server_.push_back(server);
  (void)input_file;
  (void)line;
}

std::string webservconfig::ServerCollection::GetFilePath() const
{
  return (this->file_path_);
}

const std::vector<webservconfig::Server> &webservconfig::ServerCollection::GetServer() const
{
  return (this->server_);
}

// std::pair<int, const webservconfig::ConfigBase &> webservconfig::ServerCollection::GetConfigBase(
//   std::string port, std::string address, bool is_v6, std::string hostnae,std::string path) const
// {
//   for (std::vector<Server>::const_iterator iter = this->server_.begin();
//        iter != this->server_.end(); iter++) {
//     iter->GetListenV4();
//   }
// }

// void *get_in_addr(struct sockaddr *sa)
// {
//   if (sa->sa_family == AF_INET)
//     return &(((struct sockaddr_in*)sa)->sin_addr);
//   return &(((struct sockaddr_in6*)sa)->sin6_addr);
// }

// u_short get_in_port(struct sockaddr *sa)
// {
//   if (sa->sa_family == AF_INET)
//     return ntohs(((struct sockaddr_in *)sa)->sin_port);
//   return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
// }

/**
 * !!!!!!!!!! 廃棄予定 !!!!!!!!!!
 * 
 * GetConfigBase(struct sockaddr sa, const std::string& hostname, const std::string& path);
 * 
 * 引数
 *   sa: socketにバインドしたアドレス
 *   hostname: configファイルで設定されたserver_name
 *   path: リクエストがきたURI中のpath
 * 戻り値
 *   first: configBaseの種別 0=Config(当てはまるserverが見つからない), 1=Server, 2=Location
 *   second: configBaseにキャストしたポインタ
 * 説明
 *   引数で与えられたヒントに一致するConfigBaseを返す.
 */

// std::pair<int, webservconfig::ConfigBase *> webservconfig::ServerCollection::GetConfigBase(
//   struct addrinfo ai, const std::string &hostname, const std::string &path) const
// {
//   (void)ai;
//   (void)hostname;
//   (void)path;
//   struct addrinfo *p = &ai;
//   char s[INET6_ADDRSTRLEN];
//   const char *res = inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
//   int port = get_in_port(p->ai_addr);
//   if (res) {
//     std::cout << "Requests:" << std::endl;
//     std::cout << s << std::endl;
//     std::cout << port << std::endl;
//   }
//   for (std::vector<Server>::const_iterator iter = this->server_.begin();
//        iter != this->server_.end(); iter++) {
//     webservconfig::ConfigBase::listen_type listen_vector;
//     if (p->ai_addr->sa_family == AF_INET) {
//       listen_vector = iter->GetListenV4();
//     } else {
//       listen_vector = iter->GetListenV6();
//     }
//     for (webservconfig::ConfigBase::listen_type::const_iterator it = listen_vector.begin();
//          it != listen_vector.end(); it++) {
//       std::cout << it->first << ", " << it->second << std::endl;
//       if (it->second == port && it->first == std::string(res)) {
//         std::cout << "match!!" << std::endl;
//       }
//     }
//   }
//   return std::make_pair(0, (ConfigBase *)this);
// }

std::ostream& webservconfig::ServerCollection::PutConfig(std::ostream& os) const
{
  os << "Config [" << this->file_path_ << "]" << std::endl;
  PutIndex(os, "├── ");
  PutErrorPage(os, "├── ");
  PutAutoIndex(os, "├── ");
  PutClientMaxBodySize(os, "├── ");
  PutRoot(os, "├── ");
  if (this->server_.size() != 0) {
    PutCgiExtension(os, "├── ");
  } else {
    PutCgiExtension(os, "└── ");
  }
  if (this->server_.size() != 0) {
    int size = this->server_.size();
    int i = 1;
    for (std::vector<webservconfig::Server>::const_iterator iter = this->server_.begin();
         iter != (this->server_.end()); iter++) {
      if (i != size) {
        iter->PutServer(os, "├── ", "│   ");
      } else {
        iter->PutServer(os, "└── ", "    ");
      }
      i++;
    }
  }
  return (os);
}

std::ostream& operator<<(std::ostream& os, const webservconfig::ServerCollection& config)
{
  return (config.PutConfig(os));
}
