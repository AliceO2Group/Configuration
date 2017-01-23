/// \file ConfigurationBase.h
/// \brief Base class for ConfigurationInterface implementations
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_CONFIGURATIONBASE_H_
#define ALICEO2_CONFIGURATION_SRC_CONFIGURATIONBASE_H_

#include "Configuration/ConfigurationInterface.h"

namespace AliceO2 {
namespace Configuration {

class ConfigurationBase: public ConfigurationInterface
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

    char getSeparator()
    {
      return mSeparator;
    }

    virtual Tree::Node getRecursive(const std::string& path)
    {
      throw std::runtime_error("getRecursive() unsupported by backend");
    }

  private:
    static constexpr char DEFAULT_SEPARATOR = '/';
    char mSeparator = DEFAULT_SEPARATOR;
};

} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_SRC_CONFIGURATIONBASE_H_
