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
