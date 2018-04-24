/// \file JsonBackend.cxx
/// \brief Implementation of configuration interface for JSON files
///
/// \author Pascal Boeschoten, CERN

#include "JsonBackend.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <rapidjson/document.h>     // rapidjson's DOM-style API
#include "JsonHandler.h"

namespace o2
{
namespace configuration
{
namespace backends
{
namespace
{

auto jsonToTree(const std::string& json) -> tree::Node
{
  rapidjson::Reader reader;
  rapidjson::StringStream ss(json.c_str());
  JsonHandler handler;
  reader.Parse(ss, handler);
  return tree::keyValuesToTree(handler.keyValues);
}
} // Anonymous namespace

JsonBackend::~JsonBackend()
{
}

JsonBackend::JsonBackend(const std::string& filePath)
    : mFilePath(filePath)
{
  std::ifstream stream(filePath);
  std::string json((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  mRootNode = jsonToTree(json);
  mCurrentNode = mRootNode;
}

void JsonBackend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("JsonBackend does not support putting values");
}

auto JsonBackend::getString(const std::string& path) -> Optional<std::string>
{
  const tree::Node& node = tree::getSubtree(mRootNode, path);
  return tree::get<std::string>(node);
}

auto JsonBackend::getRecursive(const std::string& path) -> tree::Node
{
  const tree::Node& node = tree::getSubtree(mCurrentNode, path);
  return node;
}

auto JsonBackend::getRecursiveMap(const std::string& path) -> KeyValueMap
{
  KeyValueMap map;
  const tree::Node& node = tree::getSubtree(mCurrentNode, path);
  const std::vector<std::pair<std::string, tree::Leaf>> keyValues = tree::treeToKeyValues(node);
  for (const auto& pair : keyValues) {
    map[pair.first] = tree::convert<std::string>(pair.second);
  }
  return map;
}

void JsonBackend::setPrefix(const std::string& path)
{
  const tree::Node& node = tree::getSubtree(mRootNode, path);
  mCurrentNode = node;
}

} // namespace backends
} // namespace configuration
} // namespace o2
