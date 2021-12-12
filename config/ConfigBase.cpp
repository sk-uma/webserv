#include "ConfigBase.hpp"

webservconfig::ConfigBase::ConfigBase():
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
  if (!this->index_flag_) {
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
  const char *char_address = line[1].c_str();

  char *char_port = std::strrchr(char_address, ':') + 1;
  if (char_port == NULL) {
    throw std::runtime_error("Port and address are not explicitly stated");
  }
  std::string address = line[1].substr(0, char_port - char_address - 1);
  std::string port(char_port);
  int port_number = strtoll(port);
  if (port_number == -1) {
    throw std::runtime_error("Invalid port");
  }
  if (address.size() >= 2 && address[0] == '[' && *(address.end() - 1) == ']') {
    address = address.substr(1, address.size() - 2);
    if (!IsComposed(address, "1234567890:"))
      throw std::runtime_error("Invalid address");
    this->v6_listen_.push_back(std::make_pair(address, port_number));
  } else if (address.size() >= 2 && address[0] != '[' && *(address.end() - 1) != ']') {
    if (!IsComposed(address, "1234567890."))
      throw std::runtime_error("Invalid address");
    this->v4_listen_.push_back(std::make_pair(address, port_number));
    // this->v4_listen_.push_back(std::pair<std::string, int>(address, port_number));
    // std::cout << &((GetListenV4().begin() - 1)->first) << std::endl;
  } else {
    throw std::runtime_error("unknown address format");
  }
}

void webservconfig::ConfigBase::InitServerName(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);
  this->server_name_ = line[1];
}

void webservconfig::ConfigBase::InitReturn(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 3, 3);

  int code = strtoll(line[1]);
  if (code < 0) {
    throw std::runtime_error("Invalid code");
  }
  this->return_ = return_type(code, line[2]);
}

void webservconfig::ConfigBase::InitErrorPage(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 3, -1);

  for (std::vector<std::string>::iterator iter = line.begin() - 1; iter != line.end(); iter++) {
    int code = strtoll(*iter);
    if (code < 0) {
      throw std::runtime_error("Invalid code");
    } else if (code < 300 || 599 < code) {
      throw std::runtime_error("code 300 ~ 599");
    } else {
      this->error_page_[code] = *(line.end() - 1);
    }
  }
}

bool webservconfig::ConfigBase::IsComposed(std::string str, std::string charset)
{
  for (std::string::iterator iter = str.begin(); iter != str.end(); iter++) {
    if (!std::strchr(charset.c_str(), *iter)) {
      return (false);
    }
  }
  return (true);
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

webservconfig::ConfigBase::error_page_type webservconfig::ConfigBase::GetErrorPage() const
{
  return (this->error_page_);
}

std::string webservconfig::ConfigBase::GetErrorPage(std::string code) const
{
  (void)code;
  return (NULL);
}

std::string webservconfig::ConfigBase::GetErrorPage(int code) const
{
  (void)code;
  return (NULL);
}

webservconfig::ConfigBase::index_type webservconfig::ConfigBase::GetIndex() const
{
  return (this->index_);
}

bool webservconfig::ConfigBase::GetAutoIndex() const
{
  return (this->autoindex_);
}

webservconfig::ConfigBase::body_size_type webservconfig::ConfigBase::GetClientMaxBodySize() const
{
  return (this->client_max_body_size_);
}

std::string webservconfig::ConfigBase::GetRoot() const
{
  return (this->root_);
}

webservconfig::ConfigBase::listen_type webservconfig::ConfigBase::GetListenV4() const
{
  return (this->v4_listen_);
}

webservconfig::ConfigBase::listen_type webservconfig::ConfigBase::GetListenV6() const
{
  return (this->v6_listen_);
}

std::string webservconfig::ConfigBase::GetServerName() const
{
  return (this->server_name_);
}

webservconfig::ConfigBase::return_type webservconfig::ConfigBase::GetReturn() const
{
  return (this->return_);
}

void webservconfig::ConfigBase::PutIndex(std::ostream &os, std::string indent) const
{
  std::string rtv;

  os << indent << "index               : ";
  if (GetIndex().size() != 0) {
    for (webservconfig::ConfigBase::index_type::iterator iter = GetIndex().begin();
         iter != (GetIndex().end() - 1); iter++) {
      os << *iter << ", ";
    }
    os << *(GetIndex().end() - 1);
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutErrorPage(std::ostream &os, std::string indent) const
{
  os << indent << "error_page          : ";
  if (GetErrorPage().size() != 0) {
    int size = GetErrorPage().size();
    for (webservconfig::ConfigBase::error_page_type::iterator iter = GetErrorPage().begin();
         iter != (GetErrorPage().end()); iter++) {
      os << "{" << iter->first << "=" << iter->second << "} ";
    }
    (void)size;
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutAutoIndex(std::ostream &os, std::string indent) const
{
  os << indent << "autoindex           : ";
  if (GetAutoIndex()) {
    os << "on";
  } else {
    os << "off";
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutClientMaxBodySize(std::ostream &os, std::string indent) const
{
  os << indent << "client_max_body_size: " << GetClientMaxBodySize() << std::endl;
}

void webservconfig::ConfigBase::PutRoot(std::ostream &os, std::string indent) const
{
  os << indent << "root                : " << GetRoot() << std::endl;
}

void webservconfig::ConfigBase::PutListenV4(std::ostream &os, std::string indent) const
{
  os << indent << "listen v4           : ";
  // std::cout << GetListenV4().size() << ", ";
  // std::cout << (GetListenV4().begin())->first << std::endl;
  if (GetListenV4().size() != 0) {
    for (listen_type::iterator iter = GetListenV4().begin(); iter != (GetListenV4().end() - 1); iter++) {
      os << iter->first << ":" << iter->second << ", ";
    }
    // std::cout << &((GetListenV4().begin())->first) << std::endl;
    os << (GetListenV4().end() - 1)->first << ":" << (GetListenV4().end() - 1)->second;
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutListenV6(std::ostream &os, std::string indent) const
{
  os << indent << "listen v6           : ";
  if (GetListenV6().size() != 0) {
    for (listen_type::iterator iter = GetListenV6().begin(); iter != (GetListenV6().end() - 1); iter++) {
      os << iter->first << ":" << iter->second << ", ";
    }
    os << (GetListenV6().end() - 1)->first << ":" << (GetListenV6().end() - 1)->second;
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutServerName(std::ostream &os, std::string indent) const
{
  os << indent << "server_name         : " << GetServerName() << std::endl;
}

void webservconfig::ConfigBase::PutReturn(std::ostream &os, std::string indent) const
{
  os << indent << "return              : " << std::endl;
}

void webservconfig::ConfigBase::PutUploadPass(std::ostream &os, std::string indent) const
{
  os << indent << "upload_pass         : xxxxx" << std::endl;
}

void webservconfig::ConfigBase::PutUploadStore(std::ostream &os, std::string indent) const
{
  os << indent << "upload_store        : xxxxxx" << std::endl;
}
