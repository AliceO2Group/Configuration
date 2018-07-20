/// \file JsonBackend.cxx
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Adam Wegrzynek, CERN

#include "JsonBackend.h"
#include <iostream>
#include <boost/property_tree/json_parser.hpp>

namespace o2
{
namespace configuration
{
namespace backends
{

Json2Backend::~Json2Backend()
{
}

Json2Backend::Json2Backend(const std::string& filePath)
    : mFilePath(filePath)
{
  if (filePath.length() == 0) {
    throw std::runtime_error("JSON filepath is empty");
  }

  try {
    boost::property_tree::read_json(filePath, mTree);
  }
  catch (const ptree_error &error) {
     throw std::runtime_error("Unable to read JSON file: " + filePath);
  }
}

void Json2Backend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("Json2Backend does not support putting values");
}

auto Json2Backend::getString(const std::string& path) -> Optional<std::string>
{
  // To use a custom separator instead of the default '.', we need to construct the path_type object explicitly
  return mTree.get_optional<std::string>(decltype(mTree)::path_type(path, getSeparator()));
}

void Json2Backend::setPrefix(const std::string& path)
{
  mFilePath = path;
}

boost::property_tree::ptree Json2Backend::getSubTree(const std::string& path)
{
  return mTree.get_child(decltype(mTree)::path_type(path, getSeparator()));
}

} // namespace configuration
} // namespace backends
} // namespace o2
