#include "gtest/gtest.h"
#include "ServerCollection.hpp"

static webservconfig::ConfigBase::listen_v4_type::value_type GetAddrV4(const std::string &address, int port) {
  struct in_addr ia;
  inet_pton(AF_INET, address.c_str(), &ia);
  return std::make_pair(ia, port);
}

static webservconfig::ConfigBase::listen_v6_type::value_type GetAddrV6(const std::string &address, int port) {
  struct in6_addr ia;
  inet_pton(AF_INET6, address.c_str(), &ia);
  return std::make_pair(ia, port);
}

bool operator==(struct in_addr left, struct in_addr right) {
  return !(std::memcmp(&left, &right, sizeof(struct in_addr)));
}

bool operator==(struct in6_addr left, struct in6_addr right) {
  return !(std::memcmp(&left, &right, sizeof(struct in6_addr)));
}

class ConfigTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    default_listen_v4.push_back(GetAddrV4("0.0.0.0", 80));
    default_index.push_back("index.html");
    default_autoindex = false;
    default_error_page.clear();
    default_size = 1048576;
    default_except["HEAD"] = true;
    default_except["POST"] = true;
    default_except["GET"] = true;
    default_except["DELETE"] = true;
    default_name.clear();
    default_return = std::make_pair(-1, "");
    default_upload_path.clear();
    default_root = "/var/www/html";
    default_cgi.clear();
  }

  virtual void TearDown() {
  }

  webservconfig::ConfigBase::listen_v4_type default_listen_v4;
  webservconfig::ConfigBase::listen_v6_type default_listen_v6;
  webservconfig::ConfigBase::index_type default_index;
  webservconfig::ConfigBase::error_page_type default_error_page;
  bool default_autoindex;
  webservconfig::ConfigBase::body_size_type default_size;
  webservconfig::ConfigBase::limit_except_type default_except;
  webservconfig::ConfigBase::server_name_list_type default_name;
  webservconfig::ConfigBase::return_type default_return;
  std::string default_upload_path;
  std::string default_root;
  webservconfig::ConfigBase::extension_list_type default_cgi;
};

TEST_F(ConfigTest, ConfigDefaultConstructor) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection();

  ASSERT_EQ(default_index, sc.GetIndex());
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  ASSERT_EQ(default_root, sc.GetRoot());
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  // ASSERT_EQ(0, sc.GetServer().size());
}

TEST_F(ConfigTest, Min000) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/000.conf");

  ASSERT_EQ(default_index, sc.GetIndex());
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  ASSERT_EQ(default_root, sc.GetRoot());
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, BeforeServerCollection001) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/001.conf");

  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  ASSERT_EQ(default_index, sc.GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  default_size = 100;
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  default_root = "/var/hoge";
  ASSERT_EQ(default_root, sc.GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, AfterServerCollection002) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/002.conf");

  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  ASSERT_EQ(default_index, sc.GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  default_size = 100;
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  default_root = "/var/hoge";
  ASSERT_EQ(default_root, sc.GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, DuplicationServerCollection003) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/003.conf");

  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  default_index.push_back("world.py");
  ASSERT_EQ(default_index, sc.GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  default_error_page[405] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  default_size = 42;
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  default_root = "/var/huga";
  ASSERT_EQ(default_root, sc.GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  default_cgi.push_back(".rb");
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, BeforeServer004) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/004.conf");

  ASSERT_EQ(default_index, sc.GetIndex());
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  ASSERT_EQ(default_root, sc.GetRoot());
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  default_listen_v4.clear();
  default_listen_v4.push_back(GetAddrV4("127.0.0.1", 80));
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  default_listen_v6.clear();
  default_listen_v6.push_back(GetAddrV6("::1", 81));
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  default_size = 100;
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  default_except["DELETE"] = false;
  default_except["POST"] = false;
  default_except["HEAD"] = false;
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  default_name.clear();
  default_name.push_back("example.com");
  default_name.push_back("example.net");
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  default_return = std::make_pair(300, "http:://127.0.0.3:83");
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  default_upload_path = std::string("/var/upload_dir");
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  default_root = "/var/hoge";
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, AfterServer005) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/005.conf");

  ASSERT_EQ(default_index, sc.GetIndex());
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  ASSERT_EQ(default_root, sc.GetRoot());
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  default_listen_v4.clear();
  default_listen_v4.push_back(GetAddrV4("127.0.0.1", 80));
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  default_listen_v6.clear();
  default_listen_v6.push_back(GetAddrV6("::1", 81));
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  default_size = 100;
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  default_except["DELETE"] = false;
  default_except["POST"] = false;
  default_except["HEAD"] = false;
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  default_name.clear();
  default_name.push_back("example.com");
  default_name.push_back("example.net");
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  default_return = std::make_pair(300, "http:://127.0.0.3:83");
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  default_upload_path = std::string("/var/upload_dir");
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  default_root = "/var/hoge";
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, DuplicationServer006) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/006.conf");

  ASSERT_EQ(default_index, sc.GetIndex());
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  ASSERT_EQ(default_root, sc.GetRoot());
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  default_listen_v4.clear();
  default_listen_v4.push_back(GetAddrV4("127.0.0.1", 80));
  default_listen_v4.push_back(GetAddrV4("127.0.0.2", 82));
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  default_listen_v6.clear();
  default_listen_v6.push_back(GetAddrV6("::1", 81));
  default_listen_v6.push_back(GetAddrV6("::", 83));
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  default_index.push_back("hoge.html");
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  default_error_page[405] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  default_size = 42;
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  default_except["DELETE"] = false;
  default_except["GET"] = false;
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  default_name.clear();
  default_name.push_back("example.com");
  default_name.push_back("example.net");
  default_name.push_back("example.co.jp");
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  default_return = std::make_pair(301, "http:://127.0.0.4:84");
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  default_upload_path = std::string("/var/dir_up");
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  default_root = "/var/huga";
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  default_cgi.push_back(".rb");
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, OnlyLocation007) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/007.conf");

  ASSERT_EQ(default_index, sc.GetIndex());
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  ASSERT_EQ(default_root, sc.GetRoot());
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  default_size = 100;
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  default_except["DELETE"] = false;
  default_except["POST"] = false;
  default_except["HEAD"] = false;
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  default_return = std::make_pair(300, "http:://127.0.0.3:83");
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  default_upload_path = std::string("/var/upload_dir");
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  default_root = "/var/hoge";
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST_F(ConfigTest, Max008) {
  webservconfig::ServerCollection sc = webservconfig::ServerCollection("gtest/example_config/008.conf");

  default_index.clear();
  default_index.push_back("hello.html");
  default_index.push_back("world.html");
  default_index.push_back("world.py");
  ASSERT_EQ(default_index, sc.GetIndex());
  default_error_page.clear();
  default_error_page[404] = "http://error/40x.html";
  default_error_page[405] = "http://error/40x.html";
  ASSERT_EQ(default_error_page, sc.GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, sc.GetAutoIndex());
  default_size = 42;
  ASSERT_EQ(default_size, sc.GetClientMaxBodySize());
  default_root = "/var/huga";
  ASSERT_EQ(default_root, sc.GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".py");
  default_cgi.push_back(".pl");
  default_cgi.push_back(".rb");
  ASSERT_EQ(default_cgi, sc.GetCgiExtension());

  std::vector<webservconfig::Server> server = sc.GetServer();

  ASSERT_EQ(1, server.size());
  default_listen_v4.clear();
  default_listen_v4.push_back(GetAddrV4("127.0.0.1", 80));
  default_listen_v4.push_back(GetAddrV4("127.0.0.2", 82));
  ASSERT_EQ(default_listen_v4, server[0].webservconfig::ConfigBase::GetListenV4());
  default_listen_v6.clear();
  default_listen_v6.push_back(GetAddrV6("::1", 81));
  default_listen_v6.push_back(GetAddrV6("::", 83));
  ASSERT_EQ(default_listen_v6, server[0].webservconfig::ConfigBase::GetListenV6());
  default_index.clear();
  default_index.push_back("hello_server.html");
  default_index.push_back("world_server.html");
  default_index.push_back("hoge_server.html");
  ASSERT_EQ(default_index, server[0].webservconfig::ConfigBase::GetIndex());
  // default_error_page.clear();
  default_error_page[406] = "http://error/40x_server.html";
  default_error_page[407] = "http://error/40x_server.html";
  ASSERT_EQ(default_error_page, server[0].webservconfig::ConfigBase::GetErrorPage());
  default_autoindex = false;
  ASSERT_EQ(default_autoindex, server[0].webservconfig::ConfigBase::GetAutoIndex());
  default_size = 100043;
  ASSERT_EQ(default_size, server[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  default_except["DELETE"] = false;
  default_except["HEAD"] = false;
  default_except["GET"] = false;
  default_except["POST"] = true;
  ASSERT_EQ(default_except, server[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  default_name.clear();
  default_name.push_back("example_server.com");
  default_name.push_back("example_server.net");
  default_name.push_back("example_server.co.jp");
  ASSERT_EQ(default_name, server[0].webservconfig::ConfigBase::GetServerName());
  default_return = std::make_pair(304, "http:://127.0.0.4:85");
  ASSERT_EQ(default_return, server[0].webservconfig::ConfigBase::GetReturn());
  default_upload_path = std::string("/var/dir_up_server");
  ASSERT_EQ(default_upload_path, server[0].webservconfig::ConfigBase::GetUploadPath());
  default_root = "/var/huga_server";
  ASSERT_EQ(default_root, server[0].webservconfig::ConfigBase::GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".out");
  default_cgi.push_back(".exe");
  ASSERT_EQ(default_cgi, server[0].webservconfig::ConfigBase::GetCgiExtension());

  std::vector<webservconfig::Location> location = server[0].GetLocation();

  ASSERT_EQ(1, location.size());
  default_index.clear();
  default_index.push_back("location.html");
  ASSERT_EQ(default_index, location[0].webservconfig::ConfigBase::GetIndex());
  // default_error_page.clear();
  default_error_page[408] = "http://error/40x_location.html";
  ASSERT_EQ(default_error_page, location[0].webservconfig::ConfigBase::GetErrorPage());
  default_autoindex = true;
  ASSERT_EQ(default_autoindex, location[0].webservconfig::ConfigBase::GetAutoIndex());
  default_size = 200;
  ASSERT_EQ(default_size, location[0].webservconfig::ConfigBase::GetClientMaxBodySize());
  default_except["DELETE"] = true;
  default_except["HEAD"] = false;
  default_except["GET"] = false;
  default_except["POST"] = false;
  ASSERT_EQ(default_except, location[0].webservconfig::ConfigBase::GetLimitExceptByDenyAll());
  default_return = std::make_pair(300, "http:://127.0.0.123:123");
  ASSERT_EQ(default_return, location[0].webservconfig::ConfigBase::GetReturn());
  default_upload_path = std::string("/var/upload_dir_location");
  ASSERT_EQ(default_upload_path, location[0].webservconfig::ConfigBase::GetUploadPath());
  default_root = "/var/hoge_location";
  ASSERT_EQ(default_root, location[0].webservconfig::ConfigBase::GetRoot());
  default_cgi.clear();
  default_cgi.push_back(".cgi");
  ASSERT_EQ(default_cgi, location[0].webservconfig::ConfigBase::GetCgiExtension());
}

TEST(ConfigException, d100) {
  EXPECT_ANY_THROW(webservconfig::ServerCollection("gtest/example_config/008.conf"));
}