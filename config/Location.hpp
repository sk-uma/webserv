#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "ConfigBase.hpp"

namespace webservconfig
{
  class Location: public ConfigBase
  {
    protected:
      std::string block_;
      std::string location_;
    
    public:
      Location();
      Location(std::string block, std::string location);
      ~Location();
      Location(const Location &other);
      const Location &operator=(const Location &location);

      void ParseLocationBlock();

      void PutLocation(std::ostream& os, std::string first_indent, std::string indent) const;

      std::string GetLocationPath() const;
  };
};

#endif
