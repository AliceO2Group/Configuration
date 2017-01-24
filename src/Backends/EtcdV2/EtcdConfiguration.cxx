/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten, CERN

#include "EtcdConfiguration.h"
#include <stdexcept>
#include "etcdcpp/etcd.hpp"
#include "etcdcpp/rapid_reply.hpp"

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace EtcdV2
{
namespace
{
/// The request key is prefixed to the response keys, this strips that from it.
auto stripRequestKey(const std::string& requestKey, const std::string& response) -> std::string
{
  assert(response.find(requestKey) == 0);
  return response.substr(requestKey.length());
}
} // Anonymous namespace

using Reply = example::RapidReply;
using Client = etcd::Client<Reply>;

struct EtcdState {
	EtcdState(const std::string& host, int port) :
			client(host, port)
	{
	}
	Client client;
};

Reply::KvPairs getReplyPairs(Reply& reply)
{
  Reply::KvPairs keyValuePairs;
  reply.GetAll(keyValuePairs);
  return keyValuePairs;
}

EtcdConfiguration::EtcdConfiguration(const std::string& host, int port)
    : mHost(host), mPort(port), mEtcdState(new EtcdState(host, port))
{
}

EtcdConfiguration::~EtcdConfiguration()
{
}

void EtcdConfiguration::setPrefix(const std::string& path)
{
  mPrefix = path;
}

void EtcdConfiguration::putString(const std::string& path, const std::string& value)
{
  mEtcdState->client.Set(addPrefix(replaceSeparator(path)), value);
}

auto EtcdConfiguration::getString(const std::string& path) -> Optional<std::string>
{
  try {
    auto reply = mEtcdState->client.Get(addPrefix(replaceSeparator(path)));
    auto replyPairs = getReplyPairs(reply);

    if (replyPairs.size() == 0) {
      return boost::none;
    }
    else if (replyPairs.size() == 1) {
      return replyPairs.begin()->second;
    }
    throw std::runtime_error("ETCD reply invalid");
  }
  catch (const etcd::ReplyException& e) {
    if (e.error_code == 100) { // Key not found error code
      // If the key was not found, we return an empty optional.
      return boost::none;
    }
    // Otherwise, we rethrow (there might be a connection issue, etc.)
    throw;
  }
}

auto EtcdConfiguration::getRecursive(const std::string& path) -> Tree::Node
{
  try {
    auto requestKey(addPrefix(replaceSeparator(path)));
    auto reply = mEtcdState->client.GetAll(addPrefix(replaceSeparator(path)));
    auto replyPairs = getReplyPairs(reply);

    std::vector<std::pair<std::string, Tree::Leaf>> keyValuePairs;

    for (const auto& pair : replyPairs) {
      keyValuePairs.emplace_back(stripRequestKey(requestKey, pair.first), pair.second);
    }

    return Tree::keyValuesToTree(keyValuePairs);
  }
  catch (const etcd::ReplyException& e) {
    if (e.error_code == 100) { // Key not found error code
      // If the key was not found, we return an empty tree.
      return Tree::Node();
    }
    // Otherwise, we rethrow (there might be a connection issue, etc.)
    throw;
  }
}

/// Prefix the prefix to the path
auto EtcdConfiguration::addPrefix(const std::string& path) -> std::string
{
  return mPrefix + path;
}

/// Replace separators in the path
auto EtcdConfiguration::replaceSeparator(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), '/', getSeparator());
  return p;
}


} // namespace EtcdV2
} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

