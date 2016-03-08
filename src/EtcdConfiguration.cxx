/*
 * EtcdConfiguration.cxx
 *
 *  Created on: Mar 7, 2016
 *      Author: pboescho
 */

#include <Configuration/EtcdConfiguration.h>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <Configuration/etcdcpp/etcd.hpp>
#include <Configuration/etcdcpp/rapid_reply.hpp>

using Reply = example::RapidReply;
using Client = etcd::Client<Reply>;

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
    : host(host), port(port)
{
}

EtcdConfiguration::~EtcdConfiguration()
{
}

void EtcdConfiguration::putString(std::string path, std::string value)
{
  Client client(host, port);
  Reply reply = client.Set(path, value);
}

std::string EtcdConfiguration::getString(std::string path)
{
  Client client(host, port);
  Reply reply = client.Get(path);
  return getReplyKeyValue(reply).second;
}
