#include "ConfigBase.hpp"

webservconfig::ConfigBase::ConfigBase():
  v4_listen_(),
  v6_listen_(),
  index_(),
  error_page_(),
  autoindex_(false),
  client_max_body_size_(1048576),
  limit_except_(),
  server_name_(),
  return_(std::make_pair(-1, "")),
  upload_path_(),
  root_("/var/www/html"),
  cgi_extension_(),
  index_flag_(false),
  cgi_extension_flag_(false)
{
  this->limit_except_["HEAD"] = true;
  this->limit_except_["GET"] = true;
  this->limit_except_["POST"] = true;
  this->limit_except_["DELETE"] = true;
}

webservconfig::ConfigBase::ConfigBase(const ConfigBase &other)
{
  *this = other;
}

webservconfig::ConfigBase::~ConfigBase()
{ }

const webservconfig::ConfigBase &webservconfig::ConfigBase::operator=(const ConfigBase &rhs)
{
  if (this != &rhs) {
    this->v4_listen_ = rhs.v4_listen_;
    this->v6_listen_ = rhs.v6_listen_;
    this->index_ = rhs.index_;
    this->error_page_ = rhs.error_page_;
    this->autoindex_ = rhs.autoindex_;
    this->client_max_body_size_ = rhs.client_max_body_size_;
    this->limit_except_ = rhs.limit_except_;
    this->server_name_ = rhs.server_name_;
    this->return_ = rhs.return_;
    this->upload_path_ = rhs.upload_path_;
    this->root_ = rhs.root_;
    this->cgi_extension_ = rhs.cgi_extension_;

    this->index_flag_ = rhs.index_flag_;
    this->cgi_extension_flag_ = rhs.cgi_extension_flag_;
  }
  return (*this);
}

/**
 * Init*
 * 
 * 引数
 *   line: 分割後のconfigファイルの1行
 * 説明
 *   メンバ引数に値をセット又は追加する
 *   内容に誤りがある場合には例外を投げる
 */

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
  } else {
    throw std::runtime_error("unknown address format");
  }
}

void webservconfig::ConfigBase::InitIndex(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 1, -1);
  if (!this->index_flag_) {
    this->index_.clear();
    this->index_flag_ = true;
  }
  if (line.size() == 1) {
    this->index_.clear();
  } else {
    this->index_.insert(this->index_.end(), line.begin() + 1, line.end());
  }
}

void webservconfig::ConfigBase::InitErrorPage(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 3, -1);

  for (std::vector<std::string>::iterator iter = line.begin() + 1; iter != line.end() - 1; iter++) {
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

void webservconfig::ConfigBase::InitLimitExceptByDenyAll(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 1, -1);

  // if (!this->limit_except_flag_) {
  //   this->limit_except_.clear();
  //   this->limit_except_flag_ = true;
  // }
  // if (line.size() == 1) {
  //   this->limit_except_.clear();
  // } else {
  //   this->limit_except_.insert(this->limit_except_.end(), line.begin() + 1, line.end());
  // }
  this->limit_except_["HEAD"] = false;
  this->limit_except_["POST"] = false;
  this->limit_except_["GET"] = false;
  this->limit_except_["DELETE"] = false;
  for (std::vector<std::string>::iterator iter = line.begin() + 1; iter != line.end(); iter++) {
    if (this->limit_except_.count(*iter) == 0) {
      std::cout << *iter << std::endl;
      throw std::runtime_error("unknown method");
    } else {
      this->limit_except_[*iter] = true;
    }
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

void webservconfig::ConfigBase::InitUploadPath(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);

  this->upload_path_ = line[1];
}

void webservconfig::ConfigBase::InitRoot(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 2, 2);
  this->root_ = line[1];
}

void webservconfig::ConfigBase::InitCgiExtension(std::vector<std::string> line)
{
  CheckNumberOfArgument(line, 1, -1);

  if (!this->cgi_extension_flag_) {
    this->cgi_extension_.clear();
    this->cgi_extension_flag_ = true;
  }
  if (line.size() == 1) {
    this->cgi_extension_.clear();
  } else {
    this->cgi_extension_.insert(this->cgi_extension_.end(), line.begin() + 1, line.end());
  }
}

/**
 * Utility関数
 */

/**
 * IsComposed
 * 
 * 引数
 *   str: 対象の文字列
 *   charset: 文字セット
 * 戻り値
 *   すべてcharsetで構成されていた場合はtrueそうでなければfalse
 * 説明
 *   strがcharset内の文字で構成されているかどうかを確認する
 */

bool webservconfig::ConfigBase::IsComposed(std::string str, std::string charset)
{
  for (std::string::iterator iter = str.begin(); iter != str.end(); iter++) {
    if (!std::strchr(charset.c_str(), *iter)) {
      return (false);
    }
  }
  return (true);
}

/**
 * CheckNumberOfArgument
 * 
 * 引数
 *   line: 分割後のconfigファイルの1行
 *   max_line: lineの許容される最大値(マイナスの場合は無視される)
 *   min_line: lineの許容される最小値(マイナスの場合は無視される)
 * 説明
 *   引数の数が適切でない場合に例外を投げる
 */

void webservconfig::ConfigBase::CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const
{
  int size = line.size();

  if ((min_size >= 0 && min_size > size) && (max_size >= 0 && max_size < size)) {
    throw std::runtime_error(std::string("invalid number of arguments in \"")
                              + line[0] + std::string("\" directive"));
  }
}

/**
 * strtoll
 * 
 * 引数
 *   str: 数値変換したい文字列
 * 戻り値
 *   返還後の数値(0>)
 *   数字以外が含まれていたまたは, long longを超えた場合(-1)
 * 説明
 *   stringをintに変換する
 */

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

/**
 * SplitLine
 * 
 * 引数
 *   line: configファイルの一行
 * 戻り値
 *   分割されたline
 * 説明
 *   一つの単語ごとに分割する
 */

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

/**
 * Setter
 */

void webservconfig::ConfigBase::SetListenV4(const webservconfig::ConfigBase::listen_type &listen) { this->v4_listen_ = listen; }
void webservconfig::ConfigBase::SetListenV6(const webservconfig::ConfigBase::listen_type &listen) { this->v6_listen_ = listen; }
void webservconfig::ConfigBase::SetIndex(const webservconfig::ConfigBase::index_type &index) { this->index_ = index; }
void webservconfig::ConfigBase::SetErrorPage(const webservconfig::ConfigBase::error_page_type &error_page) { this->error_page_ = error_page; }
void webservconfig::ConfigBase::SetAutoIndex(bool autoindex) { this->autoindex_ = autoindex; }
void webservconfig::ConfigBase::SetClientMaxBodySize(webservconfig::ConfigBase::body_size_type size) { this->client_max_body_size_ = size; }
void webservconfig::ConfigBase::SetLimitExceptByDenyAll(const limit_except_type &limit_except) { this->limit_except_ = limit_except; }
void webservconfig::ConfigBase::SetServerName(const std::string &server_name) { this->server_name_ = server_name; }
void webservconfig::ConfigBase::SetReturn(const webservconfig::ConfigBase::return_type &rt) { this->return_ = rt; }
void webservconfig::ConfigBase::SetUploadPath(const std::string &path) { this->upload_path_ = path; }
void webservconfig::ConfigBase::SetRoot(const std::string &path) { this->root_ = path; }
void webservconfig::ConfigBase::SetCgiExtension(const extension_list_type &extension) { this->cgi_extension_ = extension; }

/**
 * Getter
 */

const webservconfig::ConfigBase::listen_type &webservconfig::ConfigBase::GetListenV4() const { return (this->v4_listen_); }
const webservconfig::ConfigBase::listen_type &webservconfig::ConfigBase::GetListenV6() const { return (this->v6_listen_); }
const webservconfig::ConfigBase::index_type &webservconfig::ConfigBase::GetIndex() const { return (this->index_); }
const webservconfig::ConfigBase::error_page_type &webservconfig::ConfigBase::GetErrorPage() const { return (this->error_page_); }
bool webservconfig::ConfigBase::GetAutoIndex() const { return (this->autoindex_); }
webservconfig::ConfigBase::body_size_type webservconfig::ConfigBase::GetClientMaxBodySize() const { return (this->client_max_body_size_); }
const webservconfig::ConfigBase::limit_except_type &webservconfig::ConfigBase::GetLimitExceptByDenyAll() const { return (this->limit_except_); }
const std::string &webservconfig::ConfigBase::GetServerName() const { return (this->server_name_); }
const webservconfig::ConfigBase::return_type &webservconfig::ConfigBase::GetReturn() const { return (this->return_); }
const std::string &webservconfig::ConfigBase::GetUploadPath() const { return (this->upload_path_); }
const std::string &webservconfig::ConfigBase::GetRoot() const { return (this->root_); }
const webservconfig::ConfigBase::extension_list_type &webservconfig::ConfigBase::GetCgiExtension() const { return (this->cgi_extension_); }

/**
 * Utility Getter
 */

const std::string &webservconfig::ConfigBase::GetErrorPage(std::string code) const
{
  (void)code;
  std::string res("");
  return (this->root_);
}

const std::string &webservconfig::ConfigBase::GetErrorPage(int code) const
{
  (void)code;
  std::string res("");
  return (this->root_);
}

/**
 * 出力用関数
 */

void webservconfig::ConfigBase::PutListenV4(std::ostream &os, std::string indent) const
{
  os << indent << "listen v4           : ";
  if (this->v4_listen_.size() != 0) {
    for (listen_type::const_iterator iter = this->v4_listen_.begin(); iter != (this->v4_listen_.end() - 1); iter++) {
      os << iter->first << ":" << iter->second << ", ";
    }
    os << (this->v4_listen_.end() - 1)->first << ":" << (this->v4_listen_.end() - 1)->second;
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutListenV6(std::ostream &os, std::string indent) const
{
  os << indent << "listen v6           : ";
  if (this->v6_listen_.size() != 0) {
    for (listen_type::const_iterator iter = this->v6_listen_.begin(); iter != (this->v6_listen_.end() - 1); iter++) {
      os << iter->first << ":" << iter->second << ", ";
    }
    os << "[" << (this->v6_listen_.end() - 1)->first << "]:" << (this->v6_listen_.end() - 1)->second;
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutIndex(std::ostream &os, std::string indent) const
{
  os << indent << "index               : ";
  if (this->index_.size() != 0) {
    for (index_type::const_iterator iter = this->index_.begin();
         iter != (this->index_.end() - 1); iter++) {
      os << *iter << ", ";
    }
    os << *(this->index_.end() - 1);
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutErrorPage(std::ostream &os, std::string indent) const
{
  os << indent << "error_page          : ";
  if (this->error_page_.size() != 0) {
    int size = this->error_page_.size();
    for (error_page_type::const_iterator iter = this->error_page_.begin();
         iter != (this->error_page_.end()); iter++) {
      os << "{" << iter->first << "=" << iter->second << "} ";
    }
    (void)size;
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutAutoIndex(std::ostream &os, std::string indent) const
{
  os << indent << "autoindex           : ";
  if (this->autoindex_) {
    os << "on";
  } else {
    os << "off";
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutClientMaxBodySize(std::ostream &os, std::string indent) const
{
  os << indent << "client_max_body_size: " << this->client_max_body_size_ << std::endl;
}

void webservconfig::ConfigBase::PutLimitExceptByDenyAll(std::ostream &os, std::string indent) const
{
  bool first_flag = true;

  os << indent << "limit_except(Allow) : ";
  for (limit_except_type::const_iterator iter = this->limit_except_.begin();
       iter != this->limit_except_.end(); iter++) {
    if (iter->second) {
      if (first_flag) {
        first_flag = false;
      } else {
        os << ", ";
      }
      os << iter->first;
    }
  }
  os << std::endl;
}

void webservconfig::ConfigBase::PutServerName(std::ostream &os, std::string indent) const
{
  os << indent << "server_name         : " << this->server_name_ << std::endl;
}

void webservconfig::ConfigBase::PutReturn(std::ostream &os, std::string indent) const
{
  os << indent << "return              : " << this->return_.first << ", " << this->return_.second << std::endl;
}

void webservconfig::ConfigBase::PutUploadPath(std::ostream &os, std::string indent) const
{
  os << indent << "upload_path         : " << this->upload_path_ << std::endl;
}

void webservconfig::ConfigBase::PutRoot(std::ostream &os, std::string indent) const
{
  os << indent << "root                : " << this->root_ << std::endl;
}

void webservconfig::ConfigBase::PutCgiExtension(std::ostream &os, std::string indent) const
{
  os << indent << "cgi_extension       : ";
  if (this->cgi_extension_.size() != 0) {
    for (extension_list_type::const_iterator iter = this->cgi_extension_.begin();
         iter != (this->cgi_extension_.end() - 1); iter++) {
      os << *iter << ", ";
    }
    os << *(this->cgi_extension_.end() - 1);
  }
  os << std::endl;
}
