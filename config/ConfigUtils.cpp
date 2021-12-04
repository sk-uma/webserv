#include "ConfigUtils.hpp"

std::vector<std::string> webservconfig::SplitLine(std::string line)
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

int webservconfig::CountFrontSpace(std::string line)
{
  int len = 0;

  for (int i = 0; i < (int)line.length(); i++) {
    if (line[i] == ' ')
      len++;
    else
      break ;
  }
  return (len);
}
