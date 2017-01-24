/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef SRC_ETCDCONFIGURATION_H_
#define SRC_ETCDCONFIGURATION_H_

#include "ConfigurationBase.h"
#include <boost/scoped_ptr.hpp>

namespace AliceO2
{
namespace Configuration
{

struct EtcdState;

/// Note: since we've moved to the etcd v3 API, this class is now legacy code
class EtcdConfiguration final : public ConfigurationBase
{
  public:
    EtcdConfiguration(const std::string& host, int port);
    virtual ~EtcdConfiguration();
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

} // namespace Configuration
} // namespace AliceO2

#endif /* SRC_ETCDCONFIGURATION_H_ */
