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
  class Server;

  class Config
  {
    protected:
      typedef std::map<std::string, std::string> error_page_type;
      typedef long long                          body_size_type;
      typedef std::vector<std::string>           index_type;

  /**
   * menber var
   */

    protected:
      std::string               file_path_;
      std::vector<std::string>  index_;
      error_page_type           error_page_;
      bool                      autoindex_;
      body_size_type            client_max_body_size_;
      std::string               root_;
      bool                      index_flag_;
      std::vector<Server>       server_;

    protected:
      void            InitIndex(std::vector<std::string> line);
      void            InitAutoindex(std::vector<std::string> line);
      void            InitClientMaxBodySize(std::vector<std::string> line);
      void            InitRoot(std::vector<std::string> line);
      void            InitServer(std::vector<std::string> line, std::ifstream &input_file);

      void            CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const;
      body_size_type  strtoll(std::string str) const;

    public:

  /**
   * normalize
   */

      Config();
      Config(std::string path);
      ~Config();
      Config(const Config &other);
      const Config &operator=(const Config &rhs);

  /**
   * Getter
   */

      error_page_type GetErrorPages() const;
      std::string     GetErrorPage(std::string code) const;
      std::string     GetErrorPage(int code) const;
      std::string     GetServer() const;
      index_type      GetIndex() const;
      bool            GetAutoIndex() const;
      body_size_type  GetClientMaxBodySize() const;
      std::string     GetRoot() const;
  };
};

#endif
