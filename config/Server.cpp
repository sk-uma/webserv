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
    ConfigBase::operator=(rhs);
    this->block_ = rhs.block_;
    this->location_ = rhs.location_;
  }
  return (*this);
}

void webservconfig::Server::ParseServerBlock()
{
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
    // iter->SetListenV4(this->v4_listen_);
    // iter->SetListenV6(this->v6_listen_);
    // iter->SetListen(this->listen_);
    iter->SetListenV4(this->listen_v4_);
    iter->SetListenV6(this->listen_v6_);
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

/**
 * Getter
 */

const webservconfig::Server::index_type &webservconfig::Server::GetIndex(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetIndex());
  }
  return (ConfigBase::GetIndex());
}

const webservconfig::Server::error_page_type &webservconfig::Server::GetErrorPage(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetErrorPage());
  }
  return (ConfigBase::GetErrorPage());
}

bool webservconfig::Server::GetAutoIndex(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetAutoIndex());
  }
  return (ConfigBase::GetAutoIndex());
}

webservconfig::Server::body_size_type webservconfig::Server::GetClientMaxBodySize(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetClientMaxBodySize());
  }
  return (ConfigBase::GetClientMaxBodySize());
}

const webservconfig::Server::limit_except_type &webservconfig::Server::GetLimitExceptByDenyAll(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetLimitExceptByDenyAll());
  }
  return (ConfigBase::GetLimitExceptByDenyAll());
}

const webservconfig::Server::return_type &webservconfig::Server::GetReturn(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetReturn());
  }
  return (ConfigBase::GetReturn());
}

const std::string &webservconfig::Server::GetUploadPath(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetUploadPath());
  }
  return (ConfigBase::GetUploadPath());
}

const std::string &webservconfig::Server::GetRoot(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetRoot());
  }
  return (ConfigBase::GetRoot());
}

const webservconfig::Server::extension_list_type &webservconfig::Server::GetCgiExtension(const std::string &path) const
{
  std::pair<int, webservconfig::Location> res;

  res = this->GetLocation(path);
  if (res.first != -1) {
    return (res.second.GetCgiExtension());
  }
  return (ConfigBase::GetCgiExtension());
}

const std::vector<webservconfig::Location> &webservconfig::Server::GetLocation() const
{
  return (this->location_);
}

/**
 * Utility Getter
 */

std::pair<int, webservconfig::Location> webservconfig::Server::GetLocation(const std::string &path) const
{
  int compare_len = -1;
  webservconfig::Location compare_location;

  for (std::vector<Location>::const_iterator iter = this->location_.begin();
       iter != this->location_.end(); iter++) {
    if (!iter->GetLocationPath().compare(0, iter->GetLocationPath().size(), path)) {
      // std::cout << "Location: " << iter->GetLocationPath() << std::endl;
      if (compare_len < (int)iter->GetLocationPath().size()) {
        compare_len = iter->GetLocationPath().size();
        compare_location = *iter;
      }
    }
  }
  return std::make_pair(compare_len, compare_location);
}

std::ostream& webservconfig::Server::PutServer(std::ostream& os, std::string first_indent, std::string indent) const
{
  // os << first_indent << "Server [" << this->server_name_ << "]" << std::endl;
  os << first_indent << "Server" << std::endl;
  // PutListenV4(os, indent + "├── ");
  // PutListenV6(os, indent + "├── ");
  PutListen(os, indent + "├── ");
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
