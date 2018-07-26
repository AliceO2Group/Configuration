/// \file ConfigurationInterface.h
/// \brief Interface for putting and getting configuration parameters.
///
/// \author Pascal Boeschoten, CERN

#include "Configuration/ConfigurationInterface.h"
#include <boost/lexical_cast.hpp>

namespace o2
{
namespace configuration
{


ConfigurationInterface::~ConfigurationInterface()
{
}

// Template specializations of the convenience interface methods put/get

template<> void ConfigurationInterface::put(const std::string& path, const std::string& value)
{
  putString(path, value);
}

template<> void ConfigurationInterface::put(const std::string& path, const int& value)
{
  putString(path, boost::lexical_cast<std::string>(value));
}

template<> void ConfigurationInterface::put(const std::string& path, const double& value)
{
  putString(path, boost::lexical_cast<std::string>(value));
}

template<> auto ConfigurationInterface::get(const std::string& path) -> std::string
{
  return getString(path);
}

template<> auto ConfigurationInterface::get(const std::string& path) -> int
{
  return std::stoi(getString(path));
}

template<> auto ConfigurationInterface::get(const std::string& path) -> double
{
  return std::stod(getString(path));
}

} // namespace configuration
} // namespace o2
