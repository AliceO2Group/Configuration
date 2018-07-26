/// \file JsonBackend.cxx
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Adam Wegrzynek, CERN

#include "JsonBackend.h"
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

boost::optional<std::string> JsonBackend::getString(const std::string& path)
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

KeyValueMap JsonBackend::getRecursiveMap(const std::string& path)
{
  KeyValueMap map;
  auto subTree = mTree.get_child(decltype(mTree)::path_type(path, getSeparator()));

  // define lambda to recursively interate tree
  using boost::property_tree::ptree;
  std::function<void(const ptree&, std::string)> parse = [&](const ptree& node, std::string key) {
    map[key] = std::move(node.data());
    key = key.empty() ? "" : key + getSeparator();
    for (auto const &it: node) {
      parse(it.second, key + it.first);
    }
  };

  parse(subTree, std::string());
  return map;
}

} // namespace configuration
} // namespace backends
} // namespace o2
