#ifndef SERVERCOLLECTION_HPP
# define SERVERCOLLECTION_HPP

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
# include <netdb.h>
# include <arpa/inet.h>

# include "ConfigUtils.hpp"
# include "ConfigBase.hpp"
# include "Server.hpp"

namespace webservconfig
{
  class ServerCollection: public ConfigBase
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

      ServerCollection();
      ServerCollection(std::string path);
      ~ServerCollection();
      ServerCollection(const ServerCollection &other);
      const ServerCollection &operator=(const ServerCollection &rhs);

      std::string GetFilePath() const;
      const std::vector<Server> &GetServer() const;

      std::ostream& PutConfig(std::ostream& os) const;
      // std::pair<int, const webservconfig::ConfigBase &> GetConfigBase(std::string port,
      //   std::string address, bool is_v6, std::string hostname,std::string path) const;
      std::pair<int, webservconfig::ConfigBase *> GetConfigBase(
        struct addrinfo ai, const std::string &hostname, const std::string &path) const;
  };
};

std::ostream& operator<<(std::ostream& os, const webservconfig::ServerCollection& dt);

#endif
