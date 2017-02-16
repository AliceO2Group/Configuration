/// \file ConfigurationFactory.h
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten, CERN

#include <src/Backends/File/FileBackend.h>
#include <stdexcept>
#include "Configuration/ConfigurationFactory.h"
#ifdef FLP_CONFIGURATION_BACKEND_FILE_JSON_ENABLED
# include "Backends/Json/JsonBackend.h"
#endif
#ifdef FLP_CONFIGURATION_BACKEND_ETCD_ENABLED
# include "Backends/EtcdV2/EtcdBackend.h"
#endif
#ifdef FLP_CONFIGURATION_BACKEND_ETCDV3_ENABLED
# include "Backends/EtcdV3/EtcdV3Backend.h"
#endif
#include "UriParser/UriParser.hpp"

namespace AliceO2
{
namespace Configuration
{
namespace
{
using UniqueConfiguration = std::unique_ptr<ConfigurationInterface>;

auto getFile(const http::url& uri) -> UniqueConfiguration
{
  // If the "authority" part of the URI is missing (host, port, etc), the parser
  // will consider the thing before the first delimiter ('/') of the path as authority,
  // so we have to include that in the path we use.
  auto path = "/" + uri.host + uri.path;
  return UniqueConfiguration(new Backends::FileBackend(path));
}

auto getJson(const http::url& uri) -> UniqueConfiguration
{
  // If the "authority" part of the URI is missing (host, port, etc), the parser
  // will consider the thing before the first delimiter ('/') of the path as authority,
  // so we have to include that in the path we use.
#ifdef FLP_CONFIGURATION_BACKEND_FILE_JSON_ENABLED
  auto path = "/" + uri.host + uri.path;
  return UniqueConfiguration(new Backends::JsonBackend(path));
#else
  throw std::runtime_error("Back-end 'json' not enabled");
#endif
}

template <typename Backend>
auto getEtcd(const http::url& uri) -> UniqueConfiguration
{
  auto etcd = UniqueConfiguration(new Backend(uri.host, uri.port));
  if (!uri.path.empty()) {
    etcd->setPrefix(uri.path);
  }
  return etcd;
}

auto getEtcdV2(const http::url& uri) -> UniqueConfiguration
{
#ifdef FLP_CONFIGURATION_BACKEND_ETCD_ENABLED
  return getEtcd<Backends::EtcdBackend>(uri);
#else
  throw std::runtime_error("Back-end 'etcd-v2' not enabled");
#endif
}

auto getEtcdV3(const http::url& uri) -> UniqueConfiguration
{
#ifdef FLP_CONFIGURATION_BACKEND_ETCDV3_ENABLED
  return getEtcd<Backends::EtcdV3Backend>(uri);
#else
  throw std::runtime_error("Back-end 'etcd-v3' not enabled");
#endif
}
} // Anonymous namespace

auto ConfigurationFactory::getConfiguration(const std::string& uri) -> UniqueConfiguration
{
  auto string = uri; // The http library needs a non-const string for some reason
  http::url parsedUrl = http::ParseHttpUrl(string);

  const std::map<std::string, UniqueConfiguration(*)(const http::url&)> map = {
      {"file",    getFile},
      {"json",    getJson},
      {"etcd",    getEtcdV3},  // Default etcd is now V3
      {"etcd-v2", getEtcdV2},  // Legacy etcd option still available
      {"etcd-v3", getEtcdV3}};

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator != map.end()) {
    return iterator->second(parsedUrl);
  } else {
    throw std::runtime_error("Unrecognized backend");
  }
}

} // namespace Configuration
} // namespace AliceO2


