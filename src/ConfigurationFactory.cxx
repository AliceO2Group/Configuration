/// \file ConfigurationFactory.cxx
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten, CERN

#include <functional>
#include <map>
#include <stdexcept>
#include <Backends/Ini/IniBackend.h>
#include "Configuration/ConfigurationFactory.h"
#include "Backends/Json/JsonBackend.h"

#ifdef FLP_CONFIGURATION_BACKEND_CONSUL_ENABLED
# include "Backends/Consul/ConsulBackend.h"
#endif
#include "UriParser/UriParser.hpp"

namespace o2
{
namespace configuration
{
namespace
{
using UniqueConfiguration = std::unique_ptr<ConfigurationInterface>;

auto getIni(const http::url& uri) -> UniqueConfiguration
{
  // If the "authority" part of the URI is missing (host, port, etc), the parser
  // will consider the thing before the first delimiter ('/') of the path as authority,
  // so we have to include that in the path we use.
  auto path = "/" + uri.host + uri.path;
  return std::make_unique<backends::IniBackend>(path);
}

auto getJson(const http::url& uri) -> UniqueConfiguration
{
  auto path = "/" + uri.host + uri.path;
  return std::make_unique<backends::JsonBackend>(path);
}

auto getConsul(const http::url& uri) -> UniqueConfiguration
{
#ifdef FLP_CONFIGURATION_BACKEND_CONSUL_ENABLED
  auto consul = std::make_unique<backends::ConsulBackend>(uri.host, uri.port);
  if (!uri.path.empty()) {
    consul->setPrefix(uri.path);
  }
  return consul;
#else
  throw std::runtime_error("Back-end 'consul' not enabled");
#endif
}
} // Anonymous namespace

auto ConfigurationFactory::getConfiguration(const std::string& uri) -> UniqueConfiguration
{
  auto string = uri; // The http library needs a non-const string for some reason
  http::url parsedUrl = http::ParseHttpUrl(string);

  if (parsedUrl.protocol.empty()) {
    throw std::runtime_error("Ill-formed URI");
  }

  static const std::map<std::string, std::function<UniqueConfiguration(const http::url&)>> map = {
      {"ini", getIni},
      {"json", getJson},
      {"consul", getConsul},
  };

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator != map.end()) {
    return iterator->second(parsedUrl);
  } else {
    throw std::runtime_error("Unrecognized backend");
  }
}

} // namespace configuration
} // namespace o2


