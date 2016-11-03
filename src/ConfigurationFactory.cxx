/// \file ConfigurationFactory.h
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten, CERN

#include <stdexcept>
#include "Configuration/ConfigurationFactory.h"
#include "FileConfiguration.h"
#ifdef ALICEO2_CONFIGURATION_BACKEND_ETCD_ENABLED
# include "EtcdConfiguration.h"
#endif
#ifdef ALICEO2_CONFIGURATION_BACKEND_ETCDV3_ENABLED
# include "EtcdV3/EtcdV3Configuration.h"
#endif
#include "UriParser/UriParser.hpp"

namespace AliceO2
{
namespace Configuration
{

std::unique_ptr<ConfigurationInterface> getFile(const http::url& uri)
{
  // If the "authority" part of the URI is missing (host, port, etc), the parser
  // will consider the thing before the first delimiter ('/') of the path as authority,
  // so we have to include that in the path we use.
  auto path = uri.host + uri.path;
  return std::unique_ptr<FileConfiguration>(new FileConfiguration(path));
}

std::unique_ptr<ConfigurationInterface> getEtcd(const http::url& uri)
{
#ifdef ALICEO2_CONFIGURATION_BACKEND_ETCD_ENABLED
  auto etcd = std::unique_ptr<EtcdConfiguration>(new EtcdConfiguration(uri.host, uri.port));
  if (!uri.path.empty()) {
    etcd->setPrefix(uri.path);
  }
  return std::move(etcd);
#else
  throw std::runtime_error("Back-end 'etcd' not enabled");
#endif
}

std::unique_ptr<ConfigurationInterface> getEtcdV3(const http::url& uri)
{
#ifdef ALICEO2_CONFIGURATION_BACKEND_ETCDV3_ENABLED
  using Backend = ::AliceO2::Configuration::EtcdV3::EtcdV3Configuration;
  auto etcd = std::unique_ptr<Backend>(new Backend(uri.host, uri.port));
  if (!uri.path.empty()) {
    etcd->setPrefix(uri.path);
  }
  return std::move(etcd);
#else
  throw std::runtime_error("Back-end 'etcd-v3' not enabled");
#endif
}

std::unique_ptr<ConfigurationInterface> ConfigurationFactory::getConfiguration(const std::string& uri)
{
  auto string = uri; // The http library needs a non-const string for some reason
  http::url parsed = http::ParseHttpUrl(string);

  if (parsed.protocol == "file") {
    return getFile(parsed);
  } else if (parsed.protocol == "etcd") {
    return getEtcd(parsed);
  } else if (parsed.protocol == "etcd-v3") {
    return getEtcdV3(parsed);
  } else {
    throw std::runtime_error("Unrecognized URI scheme");
  }
}

} // namespace Configuration
} // namespace AliceO2


