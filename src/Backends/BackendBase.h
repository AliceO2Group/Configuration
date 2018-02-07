/// \file BackendBase.h
/// \brief Base class for ConfigurationInterface backend implementations
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDBASE_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDBASE_H_

#include <boost/core/noncopyable.hpp>
#include "Configuration/ConfigurationInterface.h"

namespace AliceO2 {
namespace Configuration {

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

    virtual Tree::Node getRecursive(const std::string&) override
    {
      throw std::runtime_error("getRecursive() unsupported by backend");
    }

    virtual KeyValueMap getRecursiveMap(const std::string&) override
    {
      throw std::runtime_error("getRecursiveMap() unsupported by backend");
    }

  private:
    /// Default separator for keys/paths
    static constexpr char DEFAULT_SEPARATOR = '/';

    /// Currently used separator for keys/paths
    char mSeparator = DEFAULT_SEPARATOR;
};

} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDBASE_H_
