/// \file EtcdConfiguration.cxx
/// \brief Implementation of configuration interface to the ETCD distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "EtcdV3Configuration.h"
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

namespace AliceO2 {
namespace Configuration {
namespace EtcdV3 {

EtcdV3Configuration::EtcdV3Configuration(const std::string& host, int port)
    : mChannel(grpc::CreateChannel(makeChannelString(host, port), grpc::InsecureChannelCredentials())), mStub(
        etcdserverpb::KV::NewStub(mChannel))
{
}

void EtcdV3Configuration::putString(const std::string& path, const std::string& value)
{
  grpc::ClientContext context;
  etcdserverpb::PutRequest request;
  request.set_key(prefix(path));
  request.set_value(value);
  etcdserverpb::PutResponse response;
  auto status = mStub->Put(&context, request, &response);

  if (!status.ok()) {
    //mLogger << "Failed to put key-value:{" << request.key() << "," << request.value() << "} error_code:"
    //    << status.error_code() << " error_message:" << status.error_message() << '\n' << mLogger.endm;
    throw std::runtime_error("etcd-v3 response status not OK");
  }
}

auto EtcdV3Configuration::getString(const std::string& path) -> Optional<std::string>
{
  grpc::ClientContext context;
  etcdserverpb::RangeRequest request;
  request.set_key(prefix(path));
  etcdserverpb::RangeResponse response;
  auto status = mStub->Range(&context, request, &response);

  if (!status.ok()) {
    //mLogger << "Failed to get key:" << request.key() << " error_code:"
    //    << status.error_code() << " error_message:" << status.error_message() << '\n' << mLogger.endm;
    throw std::runtime_error("etcd-v3 response status not OK");
  }

  switch (response.count()) {
    case 0:
      return boost::none;
    case 1:
      return response.kvs(0).value();
    default:
      throw std::runtime_error("etcd-v3 response count invalid");
  }
}

void EtcdV3Configuration::setPrefix(const std::string& path)
{
  mPrefix = path;
}

/// Prefix the prefix to the path
auto EtcdV3Configuration::prefix(const std::string& path) -> std::string
{
  return mPrefix + path;
}

/// Make the URI string for the channel
auto EtcdV3Configuration::makeChannelString(const std::string& host, int port) -> std::string
{
  return host + ':' + std::to_string(port);
}

} // namespace EtcdV3
} // namespace Configuration
} // namespace AliceO2
