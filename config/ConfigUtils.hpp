#ifndef CONFIGUTILS_HPP
# define CONFIGUTILS_HPP

# include <string>
# include <vector>
# include <map>
# include <vector>

# ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS 1
# endif
# ifndef EXIT_FAIL
#  define EXIT_FAIL    0
# endif

namespace webservconfig
{
  std::vector<std::string>  SplitLine(std::string line);
  int                       CountFrontSpace(std::string line);
};

#endif
