/// \file BackendBase.h
/// \brief Base class for ConfigurationInterface backend implementations
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDBASE_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDBASE_H_

#include <boost/core/noncopyable.hpp>
#include "Configuration/ConfigurationInterface.h"

namespace o2 {
namespace configuration {

/// Base class for backend implementations. Not it is non-copyable by inheriting form boost::noncopyable
class BackendBase: public ConfigurationInterface, public boost::noncopyable
{
  public:
    virtual void setPathSeparator(char separator) override
    {
      mSeparator = separator;
    }

    virtual void resetPathSeparator() override
    {
      mSeparator = DEFAULT_SEPARATOR;
    }

    /// Get the separator used for keys/paths
    char getSeparator()
    {
      return mSeparator;
    }

    virtual boost::property_tree::ptree getRecursive(const std::string&) override
    {
      throw std::runtime_error("getRecursive() unsupported by backend");
    }

    virtual KeyValueMap getRecursiveMap(const std::string&) override
    {
      throw std::runtime_error("getRecursiveMap() unsupported by backend");
    }

    virtual void setPrefix(const std::string& path) override
    {
      mPrefix = trimLeadingSlash(path) + getSeparator();
    }

  protected:
    std::string addPrefix(const std::string& path)
    {
      return mPrefix + path;
    }

    std::string trimLeadingSlash(const std::string& s)
    {
      if ((s.size() >= 1) && (s[0] == DEFAULT_SEPARATOR)) {
        return s.substr(1, s.size() - 1);
      } else {
        return s;
      }
    }

  private:
    /// Default separator for keys/paths
    static constexpr char DEFAULT_SEPARATOR = '/';

    /// Currently used separator for keys/paths
    char mSeparator = DEFAULT_SEPARATOR;

    std::string mPrefix;
};

} // namespace configuration
} // namespace o2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDBASE_H_
