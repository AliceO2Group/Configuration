/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef SRC_ETCDCONFIGURATION_H_
#define SRC_ETCDCONFIGURATION_H_

#include "Configuration/ConfigurationInterface.h"
#include <boost/scoped_ptr.hpp>

namespace AliceO2
{
namespace Configuration
{

struct EtcdState;

class EtcdConfiguration final : public ConfigurationInterface
{
  public:
    EtcdConfiguration(const std::string& host, int port);
    virtual ~EtcdConfiguration();

    virtual void putString(const std::string& path, const std::string& value) override;
    virtual Optional<std::string> getString(const std::string& path) override;

    virtual void setPrefix(const std::string& path) override;

  private:
    std::string mHost;
    int mPort;
    boost::scoped_ptr<EtcdState> mEtcdState;
    std::string mPrefix;
};

} // namespace Configuration
} // namespace AliceO2

#endif /* SRC_ETCDCONFIGURATION_H_ */
