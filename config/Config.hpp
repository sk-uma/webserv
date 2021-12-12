#ifndef CONFIG_HPP
# define CONFIG_HPP

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

# include "ConfigUtils.hpp"
# include "Server.hpp"

namespace webservconfig
{
  class Config: public ConfigBase
  {

  /**
   * menber var
   */

    protected:
      std::string           file_path_;
      std::vector<Server>   server_;

    protected:
      void            InitServer(std::vector<std::string> line, std::ifstream &input_file);

    public:

  /**
   * normalize
   */

      Config();
      Config(std::string path);
      ~Config();
      Config(const Config &other);
      const Config &operator=(const Config &rhs);

      std::string GetFilePath() const;
      const std::vector<Server> &GetServer() const;

      std::ostream& PutConfig(std::ostream& os) const;
  };
};

std::ostream& operator<<(std::ostream& os, const webservconfig::Config& dt);

#endif
