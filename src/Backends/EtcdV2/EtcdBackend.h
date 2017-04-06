/// \file EtcdBackend.h
/// \brief Configuration interface to the etcd distributed key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDS_ETCD_ETCDBACKEND_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDS_ETCD_ETCDBACKEND_H_

#include <string>
#include <boost/scoped_ptr.hpp>
#include "../BackendBase.h"

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{

struct EtcdState;

/// Backend for etcd with the v2 API.
/// Note: since we've moved to the v3 API, this class is now "legacy".
class EtcdBackend final : public BackendBase
{
  public:
    EtcdBackend(const std::string& host, int port);
    virtual ~EtcdBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual auto getRecursive(const std::string& path) -> Tree::Node override;

  private:
    auto addPrefix(const std::string& path) -> std::string;
    auto makeChannelString(const std::string& host, int port) -> std::string;
    auto replaceSeparator(const std::string& path) -> std::string;

    std::string mHost;
    int mPort;
    boost::scoped_ptr<EtcdState> mEtcdState;
    std::string mPrefix;
};

} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDS_ETCD_ETCDBACKEND_H_
