#include "ServerCollection.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <cerrno>

int main(int argc, char *argv[])
{
  std::string filename;
  std::string line;

  // const char *hostname = std::string("localhost").c_str();
  const char *hostname = std::string("127.0.0.1").c_str();
  const char *service = std::string("80").c_str();
  struct addrinfo hints, *res;
  int err;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET;

  if (argc != 2)
    return (1);

  filename = argv[1];
  try {
    webservconfig::ServerCollection config(filename);
    // std::cout << "in main: " << config.GetServer().begin()->GetListenV4().begin()->first << std::endl;
    std::cout << config;
    err = getaddrinfo(hostname, service, &hints, &res);
    std::cout << "err: " << gai_strerror(err) << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
  }
}