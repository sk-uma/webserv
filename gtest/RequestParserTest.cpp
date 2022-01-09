#include "gtest/gtest.h"
#include "RequestParser.hpp"

class RequestParserCgiEnvTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
      setenv("EXE_DIR", ".", 1);
      sc = webservconfig::ServerCollection("gtest/example_config/012.conf");
      if (sc.GetServer().size() != 0) {
        serv = sc.GetServer()[0];
      }
      request_uri_front = "GET ";
      request_uri_back = " HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.58.0\r\nAccept: */*\r\n\r\nhello";
    }

    virtual void TearDown() {
      ;
    }

    std::string request_uri_front;
    std::string request_uri_back;
    webservconfig::ServerCollection sc;
    webservconfig::Server serv;
};

TEST_F(RequestParserCgiEnvTest, ParseEnv) {
  RequestParser rp(request_uri_front + std::string("/upload/test.py/echo") + request_uri_back, serv);

  std::cout << "SCRIPT_NAME    : " << rp.get_script_name() << std::endl;
  std::cout << "PATH_INFO      : " << rp.get_path_info() << std::endl;
  std::cout << "PATH_TRANSLATED: " << rp.get_path_translated() << std::endl;
  std::cout << "QUERY_STRING   : " << rp.get_query_string() << std::endl;
}