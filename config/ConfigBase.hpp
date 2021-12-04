#ifndef CONFIGBASE_HPP
# define CONFIGBASE_HPP

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
# include <sstream>

namespace webservconfig
{
  class ConfigBase
  {
    protected:
      typedef std::map<std::string, std::string>        error_page_type;
      typedef long long                                 body_size_type;
      typedef std::vector<std::string>                  index_type;
      typedef std::vector<std::pair<std::string, int> > listen_type;
      typedef std::pair<int, std::string>               return_type;

    protected:
      std::string               file_path_;
      std::vector<std::string>  index_;
      error_page_type           error_page_;
      bool                      autoindex_;
      body_size_type            client_max_body_size_;
      std::string               root_;
      bool                      index_flag_;

      listen_type     v4_listen_;
      listen_type     v6_listen_;
      std::string     server_name_;
      return_type     return_;
      std::string     upload_pass_;
      std::string     upload_store_;

    protected:
      void            InitIndex(std::vector<std::string> line);
      void            InitAutoindex(std::vector<std::string> line);
      void            InitClientMaxBodySize(std::vector<std::string> line);
      void            InitRoot(std::vector<std::string> line);
      void            InitListen(std::vector<std::string> line);
      void            InitServerName(std::vector<std::string> line);
      void            InitReturn(std::vector<std::string> line);

      bool                      IsComposed(std::string str, std::string charset);
      void                      CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const;
      body_size_type            strtoll(std::string str) const;
      std::vector<std::string>  SplitLine(std::string line);

    public:
      ConfigBase();
      ~ConfigBase();
      ConfigBase(const ConfigBase &other);
      const ConfigBase &operator=(const ConfigBase &other);

      error_page_type GetErrorPages() const;
      std::string     GetErrorPage(std::string code) const;
      std::string     GetErrorPage(int code) const;
      std::string     GetServer() const;
      index_type      GetIndex() const;
      bool            GetAutoIndex() const;
      body_size_type  GetClientMaxBodySize() const;
      std::string     GetRoot() const;
      listen_type     GetListenV4() const;
      listen_type     GetListenV6() const;
      std::string     GetServerName() const;
      return_type     GetReturn() const;
      std::string     GetUplocaPass() const;
      std::string     GetUploadStore() const;
  };
}

#endif
