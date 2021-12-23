#include "Server.hpp"

webservconfig::Server::Server():
  ConfigBase()
{ }

webservconfig::Server::Server(std::string block):
  ConfigBase()
{
  this->block_ = block;
}

webservconfig::Server::~Server()
{ }

webservconfig::Server::Server(const Server &other)
{
  *this = other;
}

const webservconfig::Server &webservconfig::Server::operator=(const Server &rhs)
{
  if (this != &rhs) {
    // this->index_ = rhs.index_;
    // this->error_page_ = rhs.error_page_;
    // this->autoindex_ = rhs.autoindex_;
    // this->client_max_body_size_ = rhs.client_max_body_size_;
    // this->root_ = rhs.root_;
    // this->index_flag_ = rhs.index_flag_;
    // this->v4_listen_ = rhs.v4_listen_;
    // this->v6_listen_ = rhs.v6_listen_;
    // this->server_name_ = rhs.server_name_;
    // this->return_ = rhs.return_;
    // this->upload_pass_ = rhs.upload_pass_;
    // this->upload_store_ = rhs.upload_store_;
    // this->upload_path_ = rhs.upload_path_;
    ConfigBase::operator=(rhs);
    this->block_ = rhs.block_;
    this->location_ = rhs.location_;
  }
  return (*this);
}

// upload, allow_method

void webservconfig::Server::ParseServerBlock()
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
    } else if (len >= 3 && rtv[0] == "location" && rtv[2] == "{") {
      InitLocation(rtv, iss);
    } else if (rtv[0] == "listen") {
      InitListen(rtv);
    } else if (rtv[0] == "index") {
      InitIndex(rtv);
    } else if (rtv[0] == "error_page") {
      InitErrorPage(rtv);
    } else if (rtv[0] == "autoindex") {
      InitAutoindex(rtv);
    } else if (rtv[0] == "client_max_body_size") {
      InitClientMaxBodySize(rtv);
    } else if (rtv[0] == "limit_except_by_deny_all") {
      InitLimitExceptByDenyAll(rtv);
    } else if (rtv[0] == "server_name") {
      InitServerName(rtv);
    } else if (rtv[0] == "return") {
      InitReturn(rtv);
    } else if (rtv[0] == "upload_path") {
      InitUploadPath(rtv);
    } else if (rtv[0] == "root") {
      InitRoot(rtv);
    } else if (rtv[0] == "cgi_extension") {
      InitCgiExtension(rtv);
    } else {
      throw std::runtime_error(std::string("not allowed directive \"") + rtv[0] + std::string("\""));
    }
  }
  for (std::vector<Location>::iterator iter = this->location_.begin(); iter != this->location_.end(); iter++) {
    iter->SetListenV4(this->v4_listen_);
    iter->SetListenV6(this->v6_listen_);
    iter->SetIndex(this->index_);
    iter->SetErrorPage(this->error_page_);
    iter->SetAutoIndex(this->autoindex_);
    iter->SetClientMaxBodySize(this->client_max_body_size_);
    iter->SetLimitExceptByDenyAll(this->limit_except_);
    iter->SetServerName(this->server_name_);
    iter->SetReturn(this->return_);
    iter->SetUploadPath(this->upload_path_);
    iter->SetRoot(this->root_);
    iter->SetCgiExtension(this->cgi_extension_);
    iter->ParseLocationBlock();
  }
  (void)i;
  // std::cout << "init: " << (GetListenV4().begin())->first << ":" << (GetListenV4().begin())->second << std::endl;
}

void webservconfig::Server::InitLocation(std::vector<std::string> line, std::istringstream &input)
{
  std::string block, buf;

  block = "";
  while (std::getline(input, buf)) {
    if (buf == "  }") {
      break;
    } else {
      block += buf + "\n";
    }
  }
  webservconfig::Location location(block, line[1]);
  this->location_.push_back(location);
}

const std::vector<webservconfig::Location> &webservconfig::Server::GetLocation() const
{
  return (this->location_);
}

std::ostream& webservconfig::Server::PutServer(std::ostream& os, std::string first_indent, std::string indent) const
{
  os << first_indent << "Server [" << this->server_name_ << "]" << std::endl;
  // std::cout << "put: " << (GetListenV4().begin())->first << ":" << (GetListenV4().begin())->second << std::endl;
  PutListenV4(os, indent + "├── ");
  PutListenV6(os, indent + "├── ");
  PutIndex(os, indent + "├── ");
  PutErrorPage(os, indent + "├── ");
  PutAutoIndex(os, indent + "├── ");
  PutClientMaxBodySize(os, indent + "├── ");
  PutLimitExceptByDenyAll(os, indent + "├── ");
  PutServerName(os, indent + "├── ");
  PutReturn(os, indent + "├── ");
  PutUploadPath(os, indent + "├── ");
  PutRoot(os, indent + "├── ");
  if (this->location_.size() != 0) {
    PutCgiExtension(os, indent + "├── ");
  } else {
    PutCgiExtension(os, indent + "└── ");
  }
  if (this->location_.size() != 0) {
    int size = this->location_.size();
    int i = 1;
    for (std::vector<webservconfig::Location>::const_iterator iter = this->location_.begin();
         iter != this->location_.end(); iter++) {
      if (i != size) {
        iter->PutLocation(os, indent + "├── ", indent + "│   ");
      } else {
        iter->PutLocation(os, indent + "└── ", indent + "    ");
      }
      i++;
    }
  }
  return (os);
}
