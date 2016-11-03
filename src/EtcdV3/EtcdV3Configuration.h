/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_ETCDV3CONFIGURATION_H_
#define ALICEO2_CONFIGURATION_ETCDV3CONFIGURATION_H_

#include "Configuration/ConfigurationInterface.h"
#include <chrono>
#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "proto/rpc.grpc.pb.h"
#include "InfoLogger/InfoLogger.hxx"

namespace AliceO2
{
namespace Configuration
{
namespace EtcdV3
{

/// Configuration backend using the etcd v3 API
class EtcdV3Configuration final : public ConfigurationInterface
{
  public:
    EtcdV3Configuration(const std::string& host, int port)
      : mChannel(grpc::CreateChannel(makeChannelString(host, port), grpc::InsecureChannelCredentials())),
        mStub(etcdserverpb::KV::NewStub(mChannel))
    {
    }

    virtual ~EtcdV3Configuration()
    {
    }

    virtual void putString(const std::string& path, const std::string& value) override
    {
      grpc::ClientContext context;
      etcdserverpb::PutRequest request;
      request.set_key(prefix(path));
      request.set_value(value);
      etcdserverpb::PutResponse response;
      auto status = mStub->Put(&context, request, &response);

      if (!status.ok()) {
        mLogger << "Failed to put key-value:{" << request.key() << "," << request.value() << "} error_code:"
            << status.error_code() << " error_message:" << status.error_message() << '\n' << mLogger.endm;
        throw std::runtime_error("etcd-v3 response status not OK");
      }
    }

    virtual Optional<std::string> getString(const std::string& path) override
    {
      grpc::ClientContext context;
      etcdserverpb::RangeRequest request;
      request.set_key(prefix(path));
      etcdserverpb::RangeResponse response;
      auto status = mStub->Range(&context, request, &response);

      if (!status.ok()) {
        mLogger << "Failed to get key:" << request.key() << " error_code:"
            << status.error_code() << " error_message:" << status.error_message() << '\n' << mLogger.endm;
        throw std::runtime_error("etcd-v3 response status not OK");
      }

      switch (response.count()) {
        case 0: return boost::none;
        case 1: return response.kvs(0).value();
        default: throw std::runtime_error("etcd-v3 response count invalid");
      }
    }

    virtual void setPrefix(const std::string& path) override
    {
      mPrefix = path;
    }

  private:

    /// Prefix the prefix to the path
    std::string prefix(const std::string& path)
    {
      return mPrefix + path;
    }

    /// Make the URI string for the channel
    std::string makeChannelString(const std::string& host, int port)
    {
      return host + ':' + std::to_string(port);
    }

    std::string mPrefix;
    std::shared_ptr<grpc::Channel> mChannel;
    std::unique_ptr<etcdserverpb::KV::Stub> mStub;
    InfoLogger::InfoLogger mLogger;
};

} // namespace EtcdV3
} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_ETCDV3CONFIGURATION_H_
