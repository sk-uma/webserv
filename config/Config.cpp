#include "Config.hpp"

webservconfig::Config::Config():
  ConfigBase(),
  file_path_(),
  server_()
{ }

webservconfig::Config::Config(std::string path):
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
    } else if (len >= 2 && rtv[0] == "server" && len == 2 && rtv[1] == "{") {
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
  for (std::vector<Server>::iterator iter = this->server_.begin(); iter != this->server_.end(); iter++) {
    (*iter).ParseServerBlock();
  }
  (void)i;
}

webservconfig::Config::~Config()
{ }

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
