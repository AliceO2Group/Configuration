/// \file ConfigurationFactory.h
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONFACTORY_H_
#define ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONFACTORY_H_

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
    /// The URI specifies the type of the backend, its location or directory, and possibly port.
    /// Valid backends:
    ///   * "file"     file-based backend
    ///   * "etcd"     etcd default backend (currently V3)
    ///   * "etcd-v2"  etcd V2 API backend
    ///   * "etcd-v3"  etcd V3 API backend
    ///
    /// Examples of URIs:
    ///   * "etcd://myetcdserver:4001"
    ///   * "file://home/me/some/local/file.ini"
    ///   * "etcd://myetcdserver:4001/some/prefix/to/my/values"
    ///
    /// \param uri The URI
    /// \retrun A unique_ptr containing a pointer to an interface to the requested back-end
    static std::unique_ptr<ConfigurationInterface> getConfiguration(const std::string& uri);
};

} // Configuration
} // AliceO2

#endif // ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONFACTORY_H_
