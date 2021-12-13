#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <string>
# include <map>
# include <fstream>
# include <iostream>
# include <stdexcept>
# include <exception>
# include <cstring>
# include <stdlib.h>
# include <cerrno>
# include <utility>

# include "ConfigBase.hpp"
# include "Location.hpp"

namespace webservconfig
{
  class Server: public ConfigBase
  {
    protected:
      std::string block_;
      std::vector<Location> location_;

    protected:
      void  InitLocation(std::vector<std::string> line, std::istringstream &input);

    public:
      Server();
      Server(std::string block);
      ~Server();
      Server(const Server &other);
      const Server &operator=(const Server &rhs);

      void ParseServerBlock();

      std::ostream& PutServer(std::ostream& os, std::string first_indent, std::string indent) const;

      const std::vector<Location> &GetLocation() const;
  };
};

#endif
