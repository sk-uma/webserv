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
      Location(std::string block);
      ~Location();
      Location(const Location &other);
      const Location &operator=(const Location &location);

      void ParseLocationBlock();
  };
};

#endif
