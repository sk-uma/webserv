#include "Location.hpp"

webservconfig::Location::Location():
  ConfigBase()
{ }

webservconfig::Location::Location(std::string block, std::string location):
  ConfigBase()
{
  this->block_ = block;
  this->location_ = location;
}

webservconfig::Location::~Location()
{ }

webservconfig::Location::Location(const Location &other)
{
  *this = other;
}

const webservconfig::Location &webservconfig::Location::operator=(const Location &rhs)
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
    this->block_ = rhs.block_;

    this->location_ = rhs.location_;
  }
  return (*this);
}

// upload, allow_method

void webservconfig::Location::ParseLocationBlock()
{
  // std::cout << "ParseBlock" << std::endl;
  // std::cout << this->block_;
  std::istringstream iss(this->block_);
  std::string line;

  int i = 1;
  while (std::getline(iss, line)) {
    if (line.size() != 0 && *(line.end() - 1) == ';') {
      line = std::string(line.begin(), line.end() - 1);
    }
    std::vector<std::string> rtv = this->SplitLine(line);
    int len = rtv.size();
    if (len == 0) {
      continue;
    } else if (rtv[0] == "index") {
      InitIndex(rtv);
    } else if (rtv[0] == "autoindex") {
      InitAutoindex(rtv);
    } else if (rtv[0] == "client_max_body_size") {
      InitClientMaxBodySize(rtv);
    } else if (rtv[0] == "root") {
      InitRoot(rtv);
    } else if (rtv[0] == "return") {
      InitReturn(rtv);
    } else {
      throw std::runtime_error(std::string("not allowed directive \"") + rtv[0] + std::string("\""));
    }
  }
  (void)i;
}

void webservconfig::Location::PutLocation(std::ostream& os, std::string first_indent, std::string indent) const
{
  os << first_indent << "Locaton [" << this->location_ << "]" << std::endl;
  PutIndex(os, indent + "├── ");
  PutErrorPage(os, indent + "├── ");
  PutAutoIndex(os, indent + "├── ");
  PutClientMaxBodySize(os, indent + "├── ");
  // PutlimitExcept(os, indent + "├── ");
  PutReturn(os, indent + "├── ");
  PutUploadPass(os, indent + "├── ");
  PutUploadStore(os, indent + "├── ");
  PutRoot(os, indent + "└── ");
}
