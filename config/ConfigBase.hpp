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
    public:
      typedef std::map<int, std::string>                error_page_type;
      typedef long long                                 body_size_type;
      typedef std::vector<std::string>                  index_type;
      typedef std::vector<std::pair<std::string, int> > listen_type;
      typedef std::pair<int, std::string>               return_type;
      typedef std::vector<std::string>                  extension_list_type;

    protected:
      index_type                index_;
      error_page_type           error_page_;
      bool                      autoindex_;
      body_size_type            client_max_body_size_;
      std::string               root_;
      bool                      index_flag_;
      listen_type               v4_listen_;
      listen_type               v6_listen_;
      std::string               server_name_;
      return_type               return_;
      // std::string               upload_pass_;
      // std::string               upload_store_;
      std::string               upload_path_;
      extension_list_type       allow_extension_;

    protected:

/**
 * メンバ変数の初期化
 */

      void  InitIndex(std::vector<std::string> line);
      void  InitAutoindex(std::vector<std::string> line);
      void  InitClientMaxBodySize(std::vector<std::string> line);
      void  InitRoot(std::vector<std::string> line);
      void  InitListen(std::vector<std::string> line);
      void  InitServerName(std::vector<std::string> line);
      void  InitReturn(std::vector<std::string> line);
      void  InitErrorPage(std::vector<std::string> line);
      // void  InitUploadPass(std::vector<std::string> line);
      // void  InitUploadStore(std::vector<std::string> line);
      void  InitUploadPath(std::vector<std::string> line);
      void  InitAllowExtension(std::vector<std::string> line);

/**
 * Utility関数
 */

      bool                      IsComposed(std::string str, std::string charset);
      void                      CheckNumberOfArgument(std::vector<std::string> line, int min_size, int max_size) const;
      body_size_type            strtoll(std::string str) const;
      std::vector<std::string>  SplitLine(std::string line);

    public:
      ConfigBase();
      ~ConfigBase();
      ConfigBase(const ConfigBase &other);
      const ConfigBase &operator=(const ConfigBase &other);

/**
 * Getter
 */

      const error_page_type     &GetErrorPage() const;
      const index_type          &GetIndex() const;
      bool                      GetAutoIndex() const;
      body_size_type            GetClientMaxBodySize() const;
      const std::string         &GetRoot() const;
      const listen_type         &GetListenV4() const;
      const listen_type         &GetListenV6() const;
      const std::string         &GetServerName() const;
      const return_type         &GetReturn() const;
      // const std::string         &GetUploadPass() const;
      // const std::string         &GetUploadStore() const;
      const std::string         &GetUploadPath() const;
      const extension_list_type &GetAllowExtension() const;

/**
 * Utility Getter
 */

      const std::string         &GetErrorPage(std::string code) const;
      const std::string         &GetErrorPage(int code) const;

/**
 * 出力用関数
 */

      void            PutIndex(std::ostream &os, std::string indent) const;
      void            PutErrorPage(std::ostream &os, std::string indent) const;
      void            PutAutoIndex(std::ostream &os, std::string indent) const;
      void            PutClientMaxBodySize(std::ostream &os, std::string indent) const;
      void            PutRoot(std::ostream &os, std::string indent) const;
      void            PutListenV4(std::ostream &os, std::string indent) const;
      void            PutListenV6(std::ostream &os, std::string indent) const;
      void            PutServerName(std::ostream &os, std::string indent) const;
      void            PutReturn(std::ostream &os, std::string indent) const;
      // void            PutUploadPass(std::ostream &os, std::string indent) const;
      // void            PutUploadStore(std::ostream &os, std::string indent) const;
      void            PutUploadPath(std::ostream &os, std::string indent) const;
  };
}

#endif
