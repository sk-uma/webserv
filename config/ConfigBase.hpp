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
# include <netdb.h>

# include "ConfigUtils.hpp"

namespace webservconfig
{
  class ConfigBase
  {
    public:
      typedef std::vector<std::pair<std::string, std::string> > listen_string_type;
      typedef std::vector<std::pair<struct in_addr, int> >      listen_v4_type;
      typedef std::vector<std::pair<struct in6_addr, int> >     listen_v6_type;
      typedef std::vector<std::string>                          index_type;
      typedef std::map<int, std::string>                        error_page_type;
      typedef long long                                         body_size_type;
      typedef std::pair<int, std::string>                       return_type;
      typedef std::vector<std::string>                          extension_list_type;
      typedef std::map<std::string, bool>                       limit_except_type;
      typedef std::vector<std::string>                          server_name_list_type;

    protected:
      listen_string_type        listen_v4_string_;
      listen_string_type        listen_v6_string_;
      listen_v4_type            listen_v4_;
      listen_v6_type            listen_v6_;
      index_type                index_;
      error_page_type           error_page_;
      bool                      autoindex_;
      body_size_type            client_max_body_size_;
      limit_except_type         limit_except_;
      server_name_list_type     server_name_;
      return_type               return_;
      std::string               upload_path_;
      std::string               root_;
      extension_list_type       cgi_extension_;

      bool                      index_flag_;
      bool                      cgi_extension_flag_;
      bool                      server_name_flag_;

    protected:

/**
 * メンバ変数の初期化
 */

      void  InitListen(std::vector<std::string> line);
      void  InitIndex(std::vector<std::string> line);
      void  InitErrorPage(std::vector<std::string> line);
      void  InitAutoindex(std::vector<std::string> line);
      void  InitClientMaxBodySize(std::vector<std::string> line);
      void  InitLimitExceptByDenyAll(std::vector<std::string> line);
      void  InitServerName(std::vector<std::string> line);
      void  InitReturn(std::vector<std::string> line);
      void  InitUploadPath(std::vector<std::string> line);
      void  InitRoot(std::vector<std::string> line);
      void  InitCgiExtension(std::vector<std::string> line);

/**
 * Utility関数
 */

      bool                      IsComposed(std::string str, std::string charset);
      void                      CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const;
      body_size_type            strtoll(std::string str) const;
      std::vector<std::string>  SplitLine(std::string line);

    public:
      ConfigBase();
      virtual ~ConfigBase();
      ConfigBase(const ConfigBase &other);
      const ConfigBase &operator=(const ConfigBase &other);

/**
 * Setter
 */

      // void SetListenV4(const listen_type &listen);
      // void SetListenV6(const listen_type &listen);
      // void SetListen(const listen_type &listen);
      void SetListenV4(const listen_v4_type &listen);
      void SetListenV6(const listen_v6_type &listen);
      void SetIndex(const index_type &index);
      void SetErrorPage(const error_page_type &error_page);
      void SetAutoIndex(bool autoindex);
      void SetClientMaxBodySize(body_size_type size);
      void SetLimitExceptByDenyAll(const limit_except_type &limit_except);
      void SetServerName(const server_name_list_type &server_name);
      void SetReturn(const return_type &rt);
      void SetUploadPath(const std::string &path);
      void SetRoot(const std::string &path);
      void SetCgiExtension(const extension_list_type &extension);

/**
 * Getter
 */

      // const listen_type         &GetListenV4() const;
      // const listen_type         &GetListenV6() const;
      // const listen_type           &GetListen() const;
      const listen_v4_type        &GetListenV4() const;
      const listen_v6_type        &GetListenV6() const;
      const listen_string_type    &GetListenStringV4() const;
      const listen_string_type    &GetListenStringV6() const;
      const index_type            &GetIndex() const;
      const listen_string_type    &GetListenString() const;
      const error_page_type       &GetErrorPage() const;
      bool                        GetAutoIndex() const;
      body_size_type              GetClientMaxBodySize() const;
      const limit_except_type     &GetLimitExceptByDenyAll() const;
      const server_name_list_type &GetServerName() const;
      const return_type           &GetReturn() const;
      const std::string           &GetUploadPath() const;
      const std::string           &GetRoot() const;
      const extension_list_type   &GetCgiExtension() const;

/**
 * Utility Getter
 */

      const std::string         &GetErrorPage(std::string code) const;
      const std::string         &GetErrorPage(int code) const;

/**
 * 出力用関数
 */

      // void PutListenV4(std::ostream &os, std::string indent) const;
      // void PutListenV6(std::ostream &os, std::string indent) const;
      void PutListen(std::ostream &os, std::string indent) const;
      void PutIndex(std::ostream &os, std::string indent) const;
      void PutErrorPage(std::ostream &os, std::string indent) const;
      void PutAutoIndex(std::ostream &os, std::string indent) const;
      void PutClientMaxBodySize(std::ostream &os, std::string indent) const;
      void PutLimitExceptByDenyAll(std::ostream &os, std::string indent) const;
      void PutServerName(std::ostream &os, std::string indent) const;
      void PutReturn(std::ostream &os, std::string indent) const;
      void PutUploadPath(std::ostream &os, std::string indent) const;
      void PutRoot(std::ostream &os, std::string indent) const;
      void PutCgiExtension(std::ostream &os, std::string indent) const;
  };
}

#endif
