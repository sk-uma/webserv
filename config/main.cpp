#include "Config.hpp"
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

  if (argc != 2)
    return (1);

  filename = argv[1];
  try {
    webservconfig::Config config(filename);
    std::cout << config;
  } catch (const std::exception &e) {
    std::cout << "exception: " << e.what() << std::endl;
  }
}