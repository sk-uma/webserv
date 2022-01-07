#include "gtest/gtest.h"
#include "RequestParser.hpp"

TEST(RequestParserCgiEnvTest, ParseEnv) {
  setenv("EXE_DIR", ".", 1);
  RequestParser rp(std::string("GET /cgi-bin/hello/put_env.py?a=2 HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\nhello"));

  std::cout << "SCRIPT_NAME    : " << rp.get_script_name() << std::endl;
  std::cout << "PATH_INFO      : " << rp.get_path_info() << std::endl;
  std::cout << "PATH_TRANSLATED: " << rp.get_path_translated() << std::endl;
  std::cout << "QUERY_STRING   : " << rp.get_query_string() << std::endl;
}