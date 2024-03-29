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
# include "ConfigUtils.hpp"

namespace webservconfig
{
  class Server: public ConfigBase
  {
    protected:
      std::string block_;
      std::vector<Location> location_;
      Location default_location_;

    protected:
      void  InitLocation(std::vector<std::string> line, std::istringstream &input);

    public:
      Server();
      Server(std::string block);
      ~Server();
      Server(const Server &other);
      const Server &operator=(const Server &rhs);

      void ParseServerBlock();

      bool HasListen(listen_type address) const;
      bool HasServerName(const std::string &name) const;

      std::ostream& PutServer(std::ostream& os, std::string first_indent, std::string indent) const;

/**
 * Getter
 */

      const std::vector<Location> &GetLocation() const;

      const index_type            &GetIndex(const std::string &path) const;
      const error_page_type       &GetErrorPage(const std::string &path) const;
      bool                        GetAutoIndex(const std::string &path) const;
      body_size_type              GetClientMaxBodySize(const std::string &path) const;
      const limit_except_type     &GetLimitExceptByDenyAll(const std::string &path) const;
      const return_type           &GetReturn(const std::string &path) const;
      const std::string           &GetUploadPath(const std::string &path) const;
      const std::string           &GetRoot(const std::string &path) const;
      const extension_list_type   &GetCgiExtension(const std::string &path) const;

/**
 * Utility Getter
 */

      const webservconfig::Location &GetLocation(const std::string &path) const;
  };
};

#endif
