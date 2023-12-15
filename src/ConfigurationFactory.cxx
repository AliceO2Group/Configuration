// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file ConfigurationFactory.cxx
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten, CERN

#include "Configuration/ConfigurationFactory.h"
#include "Backends/Json/JsonBackend.h"
#include "Backends/String/StringBackend.h"
#include <Backends/Ini/IniBackend.h>
#include <Backends/Apricot/ApricotBackend.h>
#include <functional>
#include <map>
#include <stdexcept>
#include <filesystem>

#ifdef FLP_CONFIGURATION_BACKEND_CONSUL_ENABLED
# include "Backends/Consul/ConsulBackend.h"
#endif
#include "UriParser/UriParser.h"

namespace o2
{
namespace configuration
{
namespace
{
using UniqueConfiguration = std::unique_ptr<ConfigurationInterface>;

/// Make sure to support relative and absolute paths
auto verifyFilePath(const http::url& uri) -> std::string
{
  namespace fs = std::filesystem;
  auto relative = uri.host + uri.path;
  auto absolute = "/" + relative;
  if (fs::exists(fs::path(absolute))) {
    return absolute;
  } else if (fs::exists(fs::path(relative))) {
    return relative;
  } else {
    std::string error = "File does not exists, tried: '" + relative + "' and '" + absolute + "' ";
    throw std::runtime_error(error);
  }
  return {};
}

auto getIni(const http::url& uri) -> UniqueConfiguration
{
  return std::make_unique<backends::IniBackend>(verifyFilePath(uri));
}

auto getJson(const http::url& uri) -> UniqueConfiguration
{
  auto backend = std::make_unique<backends::JsonBackend>(verifyFilePath(uri));
  backend->readJsonFile();
  return backend;
}

auto getString(const http::url& uri) -> UniqueConfiguration
{
  auto path = uri.host + uri.path;
  auto backend = std::make_unique<backends::StringBackend>(path);
  return backend;
}

auto getApricot(const http::url& uri) -> UniqueConfiguration
{
  auto apricot = std::make_unique<backends::ApricotBackend>(uri.host, uri.port);
  if (!uri.path.empty()) {
    apricot->setBasePrefix(uri.path.substr(1));
  }
  if (!uri.search.empty()) {
    apricot->setParams(uri.search);
  }
  return apricot;
}

#ifdef FLP_CONFIGURATION_BACKEND_CONSUL_ENABLED
auto getConsul(const http::url& uri) -> UniqueConfiguration
{
  auto consul = std::make_unique<backends::ConsulBackend>(uri.host, uri.port);
  if (!uri.path.empty()) {
    consul->setBasePrefix(uri.path.substr(1));
  }
  return consul;
}

auto getConsulIni(const http::url& uri) -> UniqueConfiguration
{
  auto consul = std::make_unique<backends::ConsulBackend>(uri.host, uri.port);
  auto iniFile = consul->get<std::string>(uri.path.substr(1));
  return std::make_unique<backends::IniBackend>(iniFile, true);
}

auto getConsulJson(const http::url& uri) -> UniqueConfiguration
{
  auto consul = std::make_unique<backends::ConsulBackend>(uri.host, uri.port);
  auto jsonFile = consul->get<std::string>(uri.path.substr(1));
  auto backend = std::make_unique<backends::JsonBackend>(jsonFile);
  backend->readJsonFile(true);
  return backend;
}

#else
auto getConsul(const http::url& /*uri*/) -> UniqueConfiguration
{
  throw std::runtime_error("Back-end 'consul' not enabled");
}
auto getConsulIni(const http::url& /*uri*/) -> UniqueConfiguration
{
  throw std::runtime_error("Back-end 'consul-ini' not enabled");
}
auto getConsulJson(const http::url& /*uri*/) -> UniqueConfiguration
{
  throw std::runtime_error("Back-end 'consul-json' not enabled");
}
#endif
} // Anonymous namespace

auto ConfigurationFactory::getConfiguration(const std::string& uri) -> UniqueConfiguration
{
  auto string = uri; // The http library needs a non-const string for some reason
  http::url parsedUrl = http::ParseHttpUrl(string);

  if (parsedUrl.protocol.empty()) {
    throw std::runtime_error("Ill-formed URI");
  }

  static const std::map<std::string,
                        std::function<UniqueConfiguration(const http::url&)>>
    map = {{"ini", getIni},
           {"json", getJson},
           {"consul", getConsul},
           {"consul-ini", getConsulIni},
           {"consul-json", getConsulJson},
           {"str", getString},
           {"apricot", getApricot}};

  auto iterator = map.find(parsedUrl.protocol);
  if (iterator != map.end()) {
    return iterator->second(parsedUrl);
  } else {
    throw std::runtime_error("Unrecognized backend");
  }
}

} // namespace configuration
} // namespace o2
