/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten, CERN

#include "Configuration/EtcdConfiguration.h"
#include <stdexcept>
#include "Configuration/etcdcpp/etcd.hpp"
#include "Configuration/etcdcpp/rapid_reply.hpp"

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
    : host(host), port(port), etcdState(new EtcdState(host, port))
{
}

EtcdConfiguration::~EtcdConfiguration()
{
}

void EtcdConfiguration::putString(const std::string& path, const std::string& value)
{
  etcdState->client.Set(path, value);
}

auto EtcdConfiguration::getString(const std::string& path) -> Optional<std::string>
{
  auto reply = etcdState->client.Get(path);
  auto pairs = getReplyPairs(reply);

  if (pairs.size() == 0) {
    return boost::none;
  }
  else if (pairs.size() == 1) {
    return pairs.begin()->second;
  }
  throw std::runtime_error("ETCD reply invalid");
}

} // namespace Configuration
} // namespace AliceO2

