/// \file ConsulBackend.h
/// \brief Configuration interface to the Consul key-value store
///
/// \author Pascal Boeschoten, CERN
/// \author Adam Wegrzynek, CERN

#ifndef O2_CONFIGURATION_BACKENDS_CONSULBACKEND_H_
#define O2_CONFIGURATION_BACKENDS_CONSULBACKEND_H_

#include "../BackendBase.h"
#include <ppconsul/kv.h>
#include <string>

namespace o2
{
namespace configuration
{
namespace backends
{

/// Backend for Consul
class ConsulBackend final : public BackendBase
{
  public:
    /// Connects to Consul backend
    ConsulBackend(const std::string& host, int port);

    /// Default destructor
    virtual ~ConsulBackend() = default;
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual boost::optional<std::string> getString(const std::string& path) override;
    virtual KeyValueMap getRecursiveMap(const std::string&) override;
    virtual boost::property_tree::ptree getRecursive(const std::string& path) override;

  private:
    /// Replaces DEFAULT_SEPARATOR with '/', this is required by ppconsul
    /// \param path A path with DEFAULT_SEPARATOR
    /// \retrun A path with '/' separator
    std::string replaceDefaultWithSlash(const std::string& path);

    /// Replaces '/' with DEFAULT_SEPARATOR
    /// \param path A path with '/' separator
    /// \return A path with DEFAULT_SEPARATOR
    std::string replaceSlashWithDefault(const std::string& path);

    /// Consul endpoint
    ppconsul::Consul mConsul;

    /// Key-value object
    ppconsul::kv::Kv mStorage;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_BACKENDS_CONSULBACKEND_H_
