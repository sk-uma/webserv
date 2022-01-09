#include "ConfigUtils.hpp"
#include <iostream>

// std::vector<std::string> webservconfig::SplitLine(std::string line)
// {
//   std::vector<std::string> rtv;

//   std::string item;
//   for (int i = 0; i < (int)line.length(); i++) {
//     char ch = line[i];
//     if (ch == ' ') {
//       if (!item.empty())
//         rtv.push_back(item);
//       item.clear();
//     } else {
//       item += ch;
//     }
//   }
//   if (!item.empty())
//     rtv.push_back(item);
  
//   return (rtv);
// }

static std::string trim(const std::string& string, const char* trimCharacterList = " ")
{
  std::string result;
  std::string::size_type left = string.find_first_not_of(trimCharacterList);
  if (left != std::string::npos) {
    std::string::size_type right = string.find_last_not_of(trimCharacterList);
    result = string.substr(left, right - left + 1);
  }
  return result;
}

std::vector<std::string> webservconfig::SplitLine(std::string line)
{
  std::vector<std::string> rtv;
  bool quote_flag = false;

  std::string item;
  line = trim(line, " ");
  for (int i = 0; i < (int)line.length(); i++) {
    if (line[i] == '"') {
      if (i == 0 || line[i - 1] != '\\') {
        quote_flag = !quote_flag;
        continue ;
      } else if (i != 0 && line[i - 1] == '\\') {
        if (item.size() != 0)
          item = item.substr(0, item.size() - 1);
      }
    }
    if (line[i] == ' ' && !quote_flag) {
      // std::cout << item << std::endl;
      rtv.push_back(item);
      item.clear();
      while (line[i + 1] == ' ') {
        i++;
      }
    } else {
      // std::cout << item << std::endl;
      item += line[i];
    }
  }
  // if (!item.empty())
  //   rtv.push_back(item);
  if (line != "")
    rtv.push_back(item);

  // std::cout << "end split: " << item << std::endl;

  // for (int i = 0; i < (int)rtv.size(); i++) {
  //   std::cout << i << ": " << rtv[i] << std::endl;
  // }

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

int webservconfig::GetAddressInfo(const std::string &address, const std::string &port, struct addrinfo **dst)
{
  struct addrinfo hints;
  int res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  res = getaddrinfo(address.c_str(), port.c_str(), &hints, dst);
  return (res);
}

void *webservconfig::get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

u_short webservconfig::get_in_port(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    return ntohs(((struct sockaddr_in *)sa)->sin_port);
  return ntohs(((struct sockaddr_in6 *)sa)->sin6_port);
}

std::ostream& operator<<(std::ostream& os, struct addrinfo ai)
{
  char s[INET6_ADDRSTRLEN];
  const char *res = inet_ntop(ai.ai_family, webservconfig::get_in_addr((struct sockaddr *)ai.ai_addr), s, sizeof(s));
  if (res) {
    int port = webservconfig::get_in_port(ai.ai_addr);
    os << std::string("[") << std::string(res) << std::string("]:");
    (void)port;
  } else {
    os << std::string("invalid address");
  }
  return (os);
}
