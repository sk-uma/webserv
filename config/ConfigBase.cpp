#include "ConfigBase.hpp"

webservconfig::ConfigBase::ConfigBase():
  file_path_(""),
  index_(),
  error_page_(),
  autoindex_(false),
  client_max_body_size_(1048576),
  root_("/var/www/html"),
  index_flag_(false),
  v4_listen_(),
  v6_listen_(),
  server_name_(),
  return_(),
  upload_pass_(),
  upload_store_()
{ }

webservconfig::ConfigBase::ConfigBase(const ConfigBase &other)
{
  *this = other;
}

webservconfig::ConfigBase::~ConfigBase()
{ }

const webservconfig::ConfigBase &webservconfig::ConfigBase::operator=(const ConfigBase &rhs)
{
  if (this != &rhs) {
    this->file_path_ = rhs.file_path_;
    this->index_ = rhs.index_;
    this->error_page_ = rhs.error_page_;
    this->autoindex_ = rhs.autoindex_;
    this->client_max_body_size_ = rhs.client_max_body_size_;
    this->root_ = rhs.root_;
    this->index_flag_ = rhs.index_flag_;
    this->v4_listen_ = rhs.v4_listen_;
    this->v6_listen_ = rhs.v6_listen_;
    this->server_name_ = rhs.server_name_;
    this->return_ = rhs.return_;
    this->upload_pass_ = rhs.upload_pass_;
    this->upload_store_ = rhs.upload_store_;
  }
  return (*this);
}

void webservconfig::ConfigBase::InitIndex(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, -1);
  if (this->index_flag_) {
    this->index_.clear();
    this->index_flag_ = false;
  }
  this->index_.insert(this->index_.end(), line.begin() + 1, line.end());
}

void webservconfig::ConfigBase::InitAutoindex(std::vector<std::string> line)
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

void webservconfig::ConfigBase::InitClientMaxBodySize(std::vector<std::string> line)
{
  body_size_type body_size;

  CheckNumberOfArgument(line, 2, 2);
  if ((body_size = strtoll(line[1])) < 0) {
    throw std::runtime_error("\"client_max_body_size\" directive invalid value");
  } else {
    this->client_max_body_size_ = body_size;
  }
}

void webservconfig::ConfigBase::InitRoot(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);
  this->root_ = line[1];
}

void webservconfig::ConfigBase::InitListen(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);

  char *port = std::strrchr(line[1].c_str(), ':');
  std::cout << port << std::endl;
}

void webservconfig::ConfigBase::CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const
{
  int size = line.size();

  if ((min_size >= 0 && min_size > size) && (max_size >= 0 && max_size < size)) {
    throw std::runtime_error(std::string("invalid number of arguments in \"")
                              + line[0] + std::string("\" directive"));
  }
}

webservconfig::ConfigBase::body_size_type webservconfig::ConfigBase::strtoll(std::string str) const
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

std::vector<std::string> webservconfig::ConfigBase::SplitLine(std::string line)
{
  std::vector<std::string> rtv;

  std::string item;
  for (int i = 0; i < (int)line.length(); i++) {
    char ch = line[i];
    if (ch == ' ') {
      if (!item.empty())
        rtv.push_back(item);
      item.clear();
    } else {
      item += ch;
    }
  }
  if (!item.empty())
    rtv.push_back(item);
  
  return (rtv);
}
