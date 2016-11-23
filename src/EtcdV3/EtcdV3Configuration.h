/// \file EtcdConfiguration.h
/// \brief Definition of configuration interface to the ETCD distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_ETCDV3CONFIGURATION_H_
#define ALICEO2_CONFIGURATION_ETCDV3CONFIGURATION_H_

#include "../ConfigurationBase.h"
#include <iostream>
#include <memory>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include "proto/rpc.grpc.pb.h"
//#include "InfoLogger/InfoLogger.hxx"

namespace AliceO2
{
namespace Configuration
{
namespace EtcdV3
{

/// Configuration backend using the etcd v3 API
class EtcdV3Configuration final : public ConfigurationBase
{
  public:
    EtcdV3Configuration(const std::string& host, int port);
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual Optional<std::string> getString(const std::string& path) override;
    virtual void setPrefix(const std::string& path) override;

  private:
    auto replaceSeparator(const std::string& path) -> std::string;
    auto addPrefix(const std::string& path) -> std::string;
    auto makeChannelString(const std::string& host, int port) -> std::string;

    std::string mPrefix;
    std::shared_ptr<grpc::Channel> mChannel;
    std::unique_ptr<etcdserverpb::KV::Stub> mStub;
    //InfoLogger::InfoLogger mLogger;
};

} // namespace EtcdV3
} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_ETCDV3CONFIGURATION_H_
