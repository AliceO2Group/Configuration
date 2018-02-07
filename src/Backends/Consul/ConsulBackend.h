/// \file ConsulBackend.h
/// \brief Configuration interface to the Consul key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDS_CONSUL_CONSULBACKEND_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDS_CONSUL_CONSULBACKEND_H_

#include "../BackendBase.h"
#include <ppconsul/kv.h>
#include <string>

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{

/// Backend for Consul
class ConsulBackend final : public BackendBase
{
  public:
    ConsulBackend(const std::string& host, int port);
    virtual ~ConsulBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual auto getRecursive(const std::string& path) -> Tree::Node override;
    virtual auto getRecursiveMap(const std::string&) -> KeyValueMap override;

  private:
    auto addPrefix(const std::string& path) -> std::string;
    auto replaceSeparator(const std::string& path) -> std::string;
    auto getItems(const std::string& path) -> std::vector<ppconsul::kv::KeyValue>;

    std::string mHost;
    int mPort;
    std::string mPrefix;
    ppconsul::Consul mConsul;
    ppconsul::kv::Storage mStorage;
};

} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDS_CONSUL_CONSULBACKEND_H_
