/// \file ConfigurationInterface.h
/// \brief Interface for putting and getting configuration parameters.
///
/// \author Pascal Boeschoten, CERN

#include "Configuration/ConfigurationInterface.h"
#include <boost/lexical_cast.hpp>

namespace AliceO2
{
namespace Configuration
{


ConfigurationInterface::~ConfigurationInterface()
{
}

template <typename Out, typename OptionalIn>
boost::optional<Out> convertOptional(const OptionalIn& in)
{
  if (in) {
    return boost::lexical_cast<Out>(in.value());
  } else {
    return boost::none;
  }
}

// Default implementations of non-string puts/gets, that use putString() and
// getString() + a lexical_cast

void ConfigurationInterface::putInt(const std::string& path, int value)
{
  putString(path, boost::lexical_cast<std::string>(value));
}

void ConfigurationInterface::putFloat(const std::string& path, double value)
{
  putString(path, boost::lexical_cast<std::string>(value));
}

auto ConfigurationInterface::getInt(const std::string& path) -> Optional<int>
{
  return convertOptional<int>(getString(path));
}

auto ConfigurationInterface::getFloat(const std::string& path) -> Optional<double>
{
  return convertOptional<double>(getString(path));
}

// Template convenience interface methods

template<> void ConfigurationInterface::put(const std::string& path, const std::string& value)
{
  putString(path, value);
}

template<> void ConfigurationInterface::put(const std::string& path, int value)
{
  putInt(path, value);
}

template<> void ConfigurationInterface::put(const std::string& path, double value)
{
  putFloat(path, value);
}

template<> auto ConfigurationInterface::get(const std::string& path) -> Optional<std::string>
{
  return getString(path);
}

template<> auto ConfigurationInterface::get(const std::string& path) -> Optional<int>
{
  return getInt(path);
}

template<> auto ConfigurationInterface::get(const std::string& path) -> Optional<double>
{
  return getFloat(path);
}

} // namespace Configuration
} // namespace AliceO2
