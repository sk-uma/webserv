#include "Config.hpp"

webservconfig::Config::Config():
  file_path_(""),
  index_(),
  error_page_(),
  autoindex_(false),
  client_max_body_size_(1048576),
  root_("/var/www/html"),
  index_flag_(false),
  server_()
{ }

webservconfig::Config::Config(std::string path):
  file_path_(path),
  index_(),
  error_page_(),
  autoindex_(false),
  client_max_body_size_(1048576),
  root_("/var/www/html"),
  index_flag_(false),
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
    } else if (rtv[0] == "server" && len == 2 && rtv[1] == "{") {
      InitServer(rtv, input_file);
    } else if (rtv[0] == "index") {
      InitIndex(rtv);
    } else if (rtv[0] == "autoindex") {
      InitAutoindex(rtv);
    } else if (rtv[0] == "client_max_body_size") {
      InitClientMaxBodySize(rtv);
    } else if (rtv[0] == "root") {
      InitRoot(rtv);
    } else {
      throw std::runtime_error(std::string("not allowed directive \"") + rtv[0] + std::string("\""));
    }
  }
  this->server_[0].ParseServerBlock();
  (void)i;
}

webservconfig::Config::~Config()
{ }

void webservconfig::Config::InitIndex(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, -1);
  if (this->index_flag_) {
    this->index_.clear();
    this->index_flag_ = false;
  }
  this->index_.insert(this->index_.end(), line.begin() + 1, line.end());
}

void webservconfig::Config::InitAutoindex(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);
  if (line[1] == "on") {
    this->autoindex_ = true;
  } else if (line[1] == "off") {
    this->autoindex_ = false;
  } else {
    throw std::runtime_error("\"autoindex\" directive, it must be \"on\" or \"off\"");
  }
}

void webservconfig::Config::InitClientMaxBodySize(std::vector<std::string> line)
{
  body_size_type body_size;

  CheckNumberOfArgument(line, 2, 2);
  if ((body_size = strtoll(line[1])) < 0) {
    throw std::runtime_error("\"client_max_body_size\" directive invalid value");
  } else {
    this->client_max_body_size_ = body_size;
  }
}

void webservconfig::Config::InitRoot(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);
  this->root_ = line[1];
}

void webservconfig::Config::InitServer(std::vector<std::string> line, std::ifstream &input_file)
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
  // std::cout << "in serv" << std::endl;
  // std::cout << block << std::endl;
  webservconfig::Server server(block);
  this->server_.push_back(server);
  (void)input_file;
  (void)line;
}

void webservconfig::Config::CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const
{
  int size = line.size();

  if ((min_size >= 0 && min_size > size) && (max_size >= 0 && max_size < size)) {
    throw std::runtime_error(std::string("invalid number of arguments in \"")
                              + line[0] + std::string("\" directive"));
  }
}

webservconfig::Config::body_size_type webservconfig::Config::strtoll(std::string str) const
{
  body_size_type rtv = 0;

  for (std::string::iterator iter = str.begin(); iter != str.end(); iter++) {
    if (!std::isdigit(*iter))
      return (-1);
  }
  errno = 0;
  rtv = atoll(str.c_str());
  if (errno) {
    errno = 0;
    return (-1);
  }
  return (rtv);
}
