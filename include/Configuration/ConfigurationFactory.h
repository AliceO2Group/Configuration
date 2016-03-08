/// \file ConfigurationFactory.h
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten, CERN

#ifndef SRC_CONFIGURATIONFACTORY_H_
#define SRC_CONFIGURATIONFACTORY_H_

#include <string>
#include <memory>
#include <Configuration/ConfigurationInterface.h>

class ConfigurationFactory
{
  public:
    /// Get a ConfigurationInterface suitable for the given URI
    static std::unique_ptr<ConfigurationInterface> getConfiguration(std::string uri);
};

#endif /* SRC_CONFIGURATIONFACTORY_H_ */
