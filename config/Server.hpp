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

namespace webservconfig
{
  class Server: public ConfigBase
  {
    protected:
      std::string block_;

    public:
      Server();
      Server(std::string block);
      ~Server();
      Server(const Server &other);
      const Server &operator=(const Server &rhs);

      void ParseServerBlock();
  };
};

#endif
