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
    ConfigBase::operator=(rhs);
    this->block_ = rhs.block_;
    this->location_ = rhs.location_;
  }
  return (*this);
}

std::string webservconfig::Location::GetLocationPath() const
{
  return (this->location_);
}

void webservconfig::Location::ParseLocationBlock()
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
  (void)i;
}

void webservconfig::Location::PutLocation(std::ostream& os, std::string first_indent, std::string indent) const
{
  os << first_indent << "Locaton [" << this->location_ << "]" << std::endl;
  PutIndex(os, indent + "├── ");
  PutErrorPage(os, indent + "├── ");
  PutAutoIndex(os, indent + "├── ");
  PutClientMaxBodySize(os, indent + "├── ");
  PutLimitExceptByDenyAll(os, indent + "├── ");
  PutReturn(os, indent + "├── ");
  PutUploadPath(os, indent + "├── ");
  PutRoot(os, indent + "├── ");
  PutCgiExtension(os, indent + "└── ");
}
