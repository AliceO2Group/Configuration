/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten, CERN

#include <Configuration/EtcdConfiguration.h>
#include <stdexcept>
#include <Configuration/etcdcpp/etcd.hpp>
#include <Configuration/etcdcpp/rapid_reply.hpp>

using Reply = example::RapidReply;
using Client = etcd::Client<Reply>;

namespace AliceO2
{
namespace Configuration
{

struct EtcdState {
	EtcdState(std::string host, int port) :
			client(host, port)
	{
	}
	Client client;
};

std::pair<const std::string, std::string> getReplyKeyValue(Reply& reply)
{
  Reply::KvPairs keyValuePairs;
  reply.GetAll(keyValuePairs);

  if (keyValuePairs.size() != 1) {
    throw std::runtime_error("ETCD reply invalid");
  }

  return *keyValuePairs.begin();
}

EtcdConfiguration::EtcdConfiguration(std::string host, int port)
    : host(host), port(port), etcdState(new EtcdState(host, port))
{
}

EtcdConfiguration::~EtcdConfiguration()
{
}

void EtcdConfiguration::putString(std::string path, std::string value)
{
  etcdState->client.Set(path, value);
}

std::string EtcdConfiguration::getString(std::string path)
{
  Reply reply = etcdState->client.Get(path);
  return getReplyKeyValue(reply).second;
}

}
}