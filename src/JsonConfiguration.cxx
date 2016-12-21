/// \file JsonConfiguration.cxx
/// \brief Implementation of configuration interface for JSON files
///
/// \author Pascal Boeschoten, CERN

#include "JsonConfiguration.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <rapidjson/document.h>     // rapidjson's DOM-style API

namespace AliceO2
{
namespace Configuration
{
namespace
{

struct JsonHandler {
    std::string makeKey()
    {
      std::ostringstream stream;
      for (int i = 0; i < pathStack.size(); ++i) {
        stream << pathStack[i];
        if (i + i < pathStack.size()) {
          stream << '/';
        }
      }
      return stream.str();
    }

    // Helper function for setting values for the current leaf
    template <typename T>
    void setLeaf(T&& value)
    {
      keyValues.push_back(std::make_pair(makeKey(), value));
      popPath();
    }

    bool Null()
    {
      BOOST_THROW_EXCEPTION(std::runtime_error("null not supported"));
      return true;
    }

    bool Bool(bool value)
    {
      setLeaf(value);
      return true;
    }

    bool Int(int value)
    {
      setLeaf(value);
      return true;
    }

    bool Uint(unsigned value)
    {
      setLeaf(int(value));
      return true;
    }

    bool Int64(int64_t value)
    {
      BOOST_THROW_EXCEPTION(std::runtime_error("int64_t not supported"));
      return true;
    }

    bool Uint64(uint64_t value)
    {
      BOOST_THROW_EXCEPTION(std::runtime_error("uint64_t not supported"));
      return true;
    }

    bool Double(double value)
    {
      setLeaf(value);
      return true;
    }

    bool RawNumber(const char* str, rapidjson::SizeType length, bool copy)
    {
      BOOST_THROW_EXCEPTION(std::runtime_error("RawNumber not supported"));
      return true;
    }

    bool String(const char* str, rapidjson::SizeType length, bool copy)
    {
      setLeaf(std::string(str));
      return true;
    }

    bool StartObject()
    {
      return true;
    }

    bool Key(const char* str, rapidjson::SizeType length, bool copy)
    {
      pushPath(std::string(str));
      return true;
    }

    bool EndObject(rapidjson::SizeType memberCount)
    {
      popPath();
      return true;
    }

    bool StartArray()
    {
      BOOST_THROW_EXCEPTION(std::runtime_error("Array not supported"));
      return false;
    }

    bool EndArray(rapidjson::SizeType elementCount)
    {
      BOOST_THROW_EXCEPTION(std::runtime_error("Array not supported"));
      return false;
    }

    void pushPath(std::string path)
    {
      pathStack.push_back(path);
    }

    void popPath()
    {
      if (!pathStack.empty()) {
        pathStack.pop_back();
      }
    }

    std::vector<std::string> pathStack;
    std::vector<std::pair<std::string, Tree::Leaf>> keyValues;
};

auto jsonToTree(const std::string& json) -> Tree::Node
{
  rapidjson::Reader reader;
  rapidjson::StringStream ss(json.c_str());
  JsonHandler handler;
  reader.Parse(ss, handler);
  return Tree::keyValuesToTree(handler.keyValues);
}
} // Anonymous namespace

JsonConfiguration::~JsonConfiguration()
{
}

JsonConfiguration::JsonConfiguration(const std::string& filePath)
    : mFilePath(filePath)
{
  std::ifstream stream(filePath);
  std::string json((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
  mRootNode = jsonToTree(json);
  mCurrentNode = mRootNode;
}

void JsonConfiguration::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("JsonConfiguration does not support putting values");
}

auto JsonConfiguration::getString(const std::string& path) -> Optional<std::string>
{
  const Tree::Node& node = Tree::getSubtree(mRootNode, path);
  return Tree::get<std::string>(node);
}

auto JsonConfiguration::getRecursive(const std::string& path) -> Tree::Node
{
  const Tree::Node& node = Tree::getSubtree(mCurrentNode, path);
  return node;
}

void JsonConfiguration::setPrefix(const std::string& path)
{
  const Tree::Node& node = Tree::getSubtree(mRootNode, path);
  mCurrentNode = node;
}

} // namespace Configuration
} // namespace AliceO2
