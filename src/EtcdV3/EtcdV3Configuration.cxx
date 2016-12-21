/// \file EtcdConfiguration.cxx
/// \brief Implementation of configuration interface to the ETCD distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "EtcdV3Configuration.h"
#include <boost/algorithm/string.hpp>
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
  request.set_key(addPrefix(replaceSeparator(path)));
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
  request.set_key(addPrefix(replaceSeparator(path)));
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
auto EtcdV3Configuration::addPrefix(const std::string& path) -> std::string
{
  return mPrefix + path;
}

/// Replace separators in the path
auto EtcdV3Configuration::replaceSeparator(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), '/', getSeparator());
  return p;
}

/// Make the URI string for the channel
auto EtcdV3Configuration::makeChannelString(const std::string& host, int port) -> std::string
{
  return host + ':' + std::to_string(port);
}

auto EtcdV3Configuration::getRecursive(const std::string& path) -> Tree::Node
{
//  grpc::ClientContext context;
//  etcdserverpb::RangeRequest request;
//  request.set_key(addPrefix(replaceSeparator(path)));
//  etcdserverpb::RangeResponse response;
//  auto status = mStub->Range(&context, request, &response);
//
//  if (!status.ok()) {
//    //mLogger << "Failed to get key:" << request.key() << " error_code:"
//    //    << status.error_code() << " error_message:" << status.error_message() << '\n' << mLogger.endm;
//    throw std::runtime_error("etcd-v3 response status not OK");
//  }

  Tree::Branch tree;

//  auto getOrPutMap = [&](std::string key) {
//  };
//
//  for (auto& kv : response.kvs()) {
//    // Split key into directories
//    // Note: this can be done much more efficiently by just "walking" through the string. But this is easier for now.
//    std::vector<std::string> directories;
//    boost::split(directories, kv.key(), boost::is_any_of("/"));
//
//    // Convert directories into maps
//    _TreeMap& node = tree;
//    for (int i = 0; i < directories.size(); ++i) {
//      const auto& dir = directories[i];
//      if ((i + 1) < directories.size()) {
//        // Last iteration is a special case, because we need to put a value instead of a map
//        node[dir] = _Variant(kv.value());
//      } else {
//        if (!node.count(directories[i])) {
//          // We have to create it
//          node[dir] = _TreeMap();
//        } else {
//          node = node.at(dir);
//        }
//      }
//    }
//  }

  return tree;
}

} // namespace EtcdV3
} // namespace Configuration
} // namespace AliceO2
