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

void CompareVar(RequestParser rp,
                const std::string &script_name,
                const std::string &path_info,
                const std::string &path_translated,
                const std::string &query_string) {
  EXPECT_EQ(script_name, rp.get_script_name());
  EXPECT_EQ(path_info, rp.get_path_info());
  // EXPECT_EQ(path_translated, rp.get_path_translated());
  EXPECT_EQ(query_string, rp.get_query_string());
  (void)path_translated;
}

TEST_F(RequestParserCgiEnvTest, DefaultParseEnv) {
  RequestParser rp;

  rp = RequestParser(request_uri_front + std::string("/upload/test.py/echo") + request_uri_back, serv);
  CompareVar(rp, "/upload/test.py", "/echo", "/usr/lib/cgi-bin/echo", "");
  rp = RequestParser(request_uri_front + std::string("/upload/test.py/echo?a=4&b=2") + request_uri_back, serv);
  CompareVar(rp, "/upload/test.py", "/echo", "/usr/lib/cgi-bin/echo", "a=4&b=2");

  // std::cout << rp.get_script_name() << std::endl;
  // std::cout << rp.get_path_info() << std::endl;
  // std::cout << rp.get_path_translated() << std::endl;
  // std::cout << rp.get_query_string() << std::endl;

  rp = RequestParser(request_uri_front + std::string("/upload/test.py") + request_uri_back, serv);
  CompareVar(rp, "/upload/test.py", "", "/usr/lib/cgi-bin", "");
  rp = RequestParser(request_uri_front + std::string("/upload/test.py?a=4&b=2") + request_uri_back, serv);
  CompareVar(rp, "/upload/test.py", "", "/usr/lib/cgi-bin", "a=4&b=2");
}
