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

template<> std::string ConfigurationInterface::get(const std::string& path) throw(std::runtime_error)
{
  auto optional = getString(path);
  return (optional != boost::none) ? optional.value() : throw std::runtime_error("Could not find: " + path);
}

template<> int ConfigurationInterface::get(const std::string& path) throw(std::runtime_error)
{
  auto optional = getString(path);
  return (optional != boost::none) ? std::stoi(optional.value()) : throw std::runtime_error("Could not find: " + path);
}

template<> double ConfigurationInterface::get(const std::string& path) throw(std::runtime_error)
{
  auto optional = getString(path);
  return (optional != boost::none) ? std::stod(optional.value()) : throw std::runtime_error("Could not find: " + path);
}

template<> auto ConfigurationInterface::get(const std::string& path, const std::string& defaultValue) -> std::string
{
  auto optional = getString(path);
  return (optional != boost::none) ? optional.value() : defaultValue;
}

template<> auto ConfigurationInterface::get(const std::string& path, const int& defaultValue) -> int
{
  auto optional = getString(path);
  return (optional != boost::none) ? stoi(optional.value()) : defaultValue;
}

template<> auto ConfigurationInterface::get(const std::string& path, const double& defaultValue) -> double
{
  auto optional = getString(path);
  return (optional != boost::none) ? std::stod(optional.value()) : defaultValue;
}

} // namespace configuration
} // namespace o2
