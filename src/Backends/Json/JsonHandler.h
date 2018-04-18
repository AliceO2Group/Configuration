/// \file JsonHandler.h
/// \brief Implementation of JSON parsing handler
///
/// \author Pascal Boeschoten, CERN

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDS_JSON_JSONHANDLER_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDS_JSON_JSONHANDLER_H_

#include <exception>
#include <sstream>
#include <string>
#include <vector>
#include <boost/throw_exception.hpp>
#include <rapidjson/rapidjson.h>     // rapidjson's DOM-style API

namespace o2
{
namespace configuration
{
namespace backends
{

struct JsonHandler {
    std::string makeKey()
    {
      std::ostringstream stream;
      for (int i = 0; i < pathStack.size(); ++i) {
        stream << pathStack[i];
        if (i + 1 < pathStack.size()) {
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

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDS_JSON_JSONHANDLER_H_
