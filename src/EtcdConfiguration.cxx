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

void EtcdConfiguration::putString(const std::string& path, const std::string& value)
{
  mEtcdState->client.Set(mPrefix + path, value);
}

auto EtcdConfiguration::getString(const std::string& path) -> Optional<std::string>
{
  try {
    auto reply = mEtcdState->client.Get(mPrefix + path);
    auto pairs = getReplyPairs(reply);

    if (pairs.size() == 0) {
      return boost::none;
    }
    else if (pairs.size() == 1) {
      return pairs.begin()->second;
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

void EtcdConfiguration::setPrefix(const std::string& path)
{
  mPrefix = path;
}

} // namespace Configuration
} // namespace AliceO2

