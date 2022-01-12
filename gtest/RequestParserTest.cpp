#include "gtest/gtest.h"
#include "RequestParser.hpp"

class RequestParserTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
      setenv("EXE_DIR", ".", 1);
      sc = webservconfig::ServerCollection("gtest/example_config/012.conf");
      if (sc.GetServer().size() != 0) {
        serv = sc.GetServer()[0];
      }
      request_line = "GET . HTTP/1.1\r\n";
      header = "Host: localhost\r\nUser-Agent: cpp\r\nAccept: */*";
      split_str = "\r\n\r\n";
      body = "hello";
      request_uri_front = "GET ";
      request_uri_back = " HTTP/1.1\r\nHost: localhost\r\nUser-Agent: cpp\r\nAccept: */*\r\n\r\nhello";
    }

    virtual void TearDown() {
      ;
    }

    std::string request_uri_front;
    std::string request_uri_back;
    std::string request_line;
    std::string header;
    std::string body;
    std::string split_str;
    webservconfig::ServerCollection sc;
    webservconfig::Server serv;
};

static void CompareCgiVar(const RequestParser &rp,
                          const std::string &script_name,
                          const std::string &path_info,
                          const std::string &path_translated,
                          const std::string &query_string) {
  EXPECT_EQ(script_name, rp.get_script_name());
  EXPECT_EQ(path_info, rp.get_path_info());
  EXPECT_EQ(path_translated, rp.get_path_translated());
  EXPECT_EQ(query_string, rp.get_query_string());
  (void)path_translated;
}

TEST_F(RequestParserTest, UriParse) {
  RequestParser rp;

  rp = RequestParser(request_uri_front + std::string("/upload/test.py/echo") + request_uri_back, serv);
  CompareCgiVar(rp, "/upload/test.py", "/echo", "/usr/lib/cgi-bin/echo", "");
  rp = RequestParser(request_uri_front + std::string("/upload/test.py/echo?a=4&b=2") + request_uri_back, serv);
  CompareCgiVar(rp, "/upload/test.py", "/echo", "/usr/lib/cgi-bin/echo", "a=4&b=2");
  rp = RequestParser(request_uri_front + std::string("/upload/test.py") + request_uri_back, serv);
  CompareCgiVar(rp, "/upload/test.py", "", "/usr/lib/cgi-bin", "");
  rp = RequestParser(request_uri_front + std::string("/upload/test.py?a=4&b=2") + request_uri_back, serv);
  CompareCgiVar(rp, "/upload/test.py", "", "/usr/lib/cgi-bin", "a=4&b=2");
}

static void CompareHeaderVar(const RequestParser &rp,
                             const std::string &method,
                             const std::string &uri) {
  EXPECT_EQ(method, rp.get_method());
  EXPECT_EQ(uri, rp.get_uri());
}

TEST_F(RequestParserTest, RequestsLineParse) {
  RequestParser rp;

  rp = RequestParser("GET / HTTP/1.1" + header + split_str + body, serv);
  CompareHeaderVar(rp, "GET", "/");
}
