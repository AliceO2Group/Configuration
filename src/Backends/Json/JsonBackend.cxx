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

JsonBackend::~JsonBackend()
{
}

JsonBackend::JsonBackend(const std::string& filePath)
    : mFilePath(filePath)
{
  if (filePath.length() == 0) {
    throw std::runtime_error("JSON filepath is empty");
  }

  try {
    boost::property_tree::read_json(filePath, mTree);
  }
  catch (const boost::property_tree::ptree_error &error) {
     throw std::runtime_error("Unable to read JSON file: " + filePath);
  }
}

void JsonBackend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("JsonBackend does not support putting values");
}

auto JsonBackend::getString(const std::string& path) -> Optional<std::string>
{
  // To use a custom separator instead of the default '.', we need to construct the path_type object explicitly
  return mTree.get_optional<std::string>(decltype(mTree)::path_type(path, getSeparator()));
}

void JsonBackend::setPrefix(const std::string& path)
{
  mFilePath = path;
}

boost::property_tree::ptree JsonBackend::getRecursive(const std::string& path)
{
  return mTree.get_child(decltype(mTree)::path_type(path, getSeparator()));
}

auto JsonBackend::getRecursiveMap(const std::string& path) -> KeyValueMap
{
  auto subTree = mTree.get_child(decltype(mTree)::path_type(path, getSeparator()));
  KeyValueMap map;

  // define lambda to recursively interate tree
  std::function<void(const boost::property_tree::ptree&, std::string)> parseTree;
  parseTree = [&](const boost::property_tree::ptree& pt, std::string key) {
    std::string nkey;
    if (!key.empty()) {
      if (pt.data().size() != 0) {
        map[key] = std::move(pt.data());
      }
      nkey = key + getSeparator();
    }
    for (auto const &it: pt) {
      parseTree(it.second, nkey + it.first);
    }
  };

  parseTree(subTree, "");
  return map;
}

} // namespace configuration
} // namespace backends
} // namespace o2
