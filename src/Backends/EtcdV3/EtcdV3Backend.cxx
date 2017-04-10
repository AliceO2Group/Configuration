/// \file EtcdBackend.cxx
/// \brief Implementation of configuration interface to the etcd distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "EtcdV3Backend.h"
#include <boost/algorithm/string.hpp>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <sstream>

using namespace std::string_literals;

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace
{
void setRangeRequestGetRecursive(std::string key, etcdserverpb::RangeRequest& request)
{
  request.set_key(key);

  // To do a recursive get (or "prefix" get) we need to give the same key to the range_end, except with the last byte
  // incremented by 1.
  // See: https://github.com/coreos/etcd/blob/master/Documentation/dev-guide/api_reference_v3.md
  if (!key.empty()) {
    key[key.size() - 1]++;
  }
  request.set_range_end(key);
}

/// The request key is prefixed to the response keys, this strips that from it.
auto stripRequestKey(const std::string& requestKey, const std::string& response) -> std::string
{
  assert(response.find(requestKey) == 0);
  return response.substr(requestKey.length());
}
} // Anonymous namespace

EtcdV3Backend::EtcdV3Backend(const std::string& host, int port)
    : mChannel(grpc::CreateChannel(makeChannelString(host, port), grpc::InsecureChannelCredentials())), mStub(
        etcdserverpb::KV::NewStub(mChannel))
{
  // Test connection (it throws if the connection is bad)
  getString("/");
}

void EtcdV3Backend::putString(const std::string& path, const std::string& value)
{
  grpc::ClientContext context;
  etcdserverpb::PutRequest request;
  request.set_key(addPrefix(replaceSeparator(path)));
  request.set_value(value);
  etcdserverpb::PutResponse response;
  auto status = mStub->Put(&context, request, &response);

  if (!status.ok()) {
    std::stringstream stream;
    stream << "etcd-v3 response status not OK, failed to put key-value:{" << request.key() << "," << request.value()
        << "} error_code:" << status.error_code() << " error_message:" << status.error_message();
    throw std::runtime_error(stream.str());
  }
}

auto EtcdV3Backend::getString(const std::string& path) -> Optional<std::string>
{
  grpc::ClientContext context;
  etcdserverpb::RangeRequest request;
  request.set_key(addPrefix(replaceSeparator(path)));
  etcdserverpb::RangeResponse response;
  auto status = mStub->Range(&context, request, &response);

  if (!status.ok()) {
    std::stringstream stream;
    stream << "etcd-v3 response status not OK, failed to get key:" << request.key() << " error_code:"
        << status.error_code() << " error_message:" << status.error_message();
    throw std::runtime_error(stream.str());
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

auto EtcdV3Backend::getRecursive(const std::string& path) -> Tree::Node
{
  grpc::ClientContext context;
  etcdserverpb::RangeRequest request;
  auto requestKey = addPrefix(replaceSeparator(path));
  setRangeRequestGetRecursive(requestKey, request);

  etcdserverpb::RangeResponse response;
  auto status = mStub->Range(&context, request, &response);

  if (!status.ok()) {
    std::stringstream stream;
    stream << "etcd-v3 response status not OK, failed to get key:" << request.key() << " error_code:"
        << status.error_code() << " error_message:" << status.error_message();
    throw std::runtime_error(stream.str());
  }

  std::vector<std::pair<std::string, Tree::Leaf>> keyValuePairs;
  for (int i = 0; i < response.count(); ++i) {
    auto strippedKey = stripRequestKey(requestKey, response.kvs(i).key());
    keyValuePairs.emplace_back(strippedKey, response.kvs(i).value());
  }

  return Tree::keyValuesToTree(keyValuePairs);
}

void EtcdV3Backend::setPrefix(const std::string& path)
{
  mPrefix = path;
}

/// Prefix the prefix to the path
auto EtcdV3Backend::addPrefix(const std::string& path) -> std::string
{
  return mPrefix + path;
}

/// Replace separators in the path
auto EtcdV3Backend::replaceSeparator(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), '/', getSeparator());
  return p;
}

/// Make the URI string for the channel
auto EtcdV3Backend::makeChannelString(const std::string& host, int port) -> std::string
{
  return host + ':' + std::to_string(port);
}

} // namespace Backends
} // namespace Configuration
} // namespace AliceO2
