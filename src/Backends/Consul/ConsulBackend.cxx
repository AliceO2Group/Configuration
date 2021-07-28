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

/// \file ConsulBackend.h
/// \brief Configuration interface to the Consul key-value store
///
/// \author Pascal Boeschoten, CERN

#include "ConsulBackend.h"

namespace o2
{
namespace configuration
{
namespace backends
{
namespace
{

auto stripRequestKey(const std::string& requestKey, const std::string& response) -> std::string
{
  int length = requestKey.length();
  assert(response.find(requestKey) == 0);
  if (requestKey.back() != '/') {
    length++;
  }
  return response.substr(length);
}
} // Anonymous namespace

ConsulBackend::ConsulBackend(const std::string& host, int port) :
    mConsul(host + ":" + std::to_string(port)), mStorage(mConsul)
{
}

auto ConsulBackend::replaceDefaultWithSlash(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), getSeparator(), '/');
  return p;
}

auto ConsulBackend::replaceSlashWithDefault(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), '/', getSeparator());
  return p;
}

void ConsulBackend::putString(const std::string& path, const std::string& value)
{
  mStorage.set(replaceDefaultWithSlash(addPrefix(path)), value);
}

boost::optional<std::string> ConsulBackend::getString(const std::string& path)
{
  auto item = mStorage.item(replaceDefaultWithSlash(addConsulPrefix(path)),
      ppconsul::kw::consistency = ppconsul::Consistency::Stale);
  if (item.valid()) {
    return std::move(item.value);
  } else {
    return {};
  }
}

boost::property_tree::ptree ConsulBackend::getRecursive(const std::string& path)
{
  auto requestKey = replaceDefaultWithSlash(addConsulPrefix(path));
  auto items = mStorage.items(requestKey, ppconsul::kw::consistency = ppconsul::Consistency::Stale);
  boost::property_tree::ptree tree;
  for (const auto& item : items) {
    tree.put(replaceSlashWithDefault(stripRequestKey(requestKey, item.key)), std::move(item.value));
  }
  return tree;
}

KeyValueMap ConsulBackend::getRecursiveMap(const std::string& path)
{
  auto requestKey = replaceDefaultWithSlash(addConsulPrefix(path));
  auto items = mStorage.items(requestKey, ppconsul::kw::consistency = ppconsul::Consistency::Stale);
  KeyValueMap map;
  for (const auto& item : items) {
    if (item.value.size() == 0) {
      continue;
    }
    map[replaceSlashWithDefault(stripRequestKey(requestKey, item.key))] = std::move(item.value);
  }
  return map;
}

} // namespace backends
} // namespace configuration
} // namespace o2
