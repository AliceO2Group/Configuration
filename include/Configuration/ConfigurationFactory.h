/// \file ConfigurationFactory.h
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef SRC_CONFIGURATIONFACTORY_H_
#define SRC_CONFIGURATIONFACTORY_H_

#include <string>
#include <memory>
#include "Configuration/ConfigurationInterface.h"

namespace AliceO2
{
namespace Configuration
{

class ConfigurationFactory
{
  public:
    /// Get a ConfigurationInterface suitable for the given URI
    /// The URI specifies the type of the backend, its location or directory, and possibly port
    /// Examples of URIs:
    ///  - "etcd://myetcdserver:4001"
    ///  - "file://home/me/some/local/file.ini"
    /// \param uri The URI
    /// \retrun A unique_ptr containing a pointer to an interface to the requested back-end
    static std::unique_ptr<ConfigurationInterface> getConfiguration(std::string uri);
};

} // Configuration
} // AliceO2

#endif /* SRC_CONFIGURATIONFACTORY_H_ */
