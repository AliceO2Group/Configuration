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
std::string trimLeadingSlash(const std::string& s)
{
  if ((s.size() >= 1) && (s[0] == '/')) {
    return s.substr(1, s.size() - 1);
  } else {
    return s;
  }
}

/// The request key is prefixed to the response keys, this strips that from it.
auto stripRequestKey(const std::string& requestKey, const std::string& response) -> std::string
{
  assert(response.find(requestKey) == 0);
  return response.substr(requestKey.length());
}
} // Anonymous namespace

ConsulBackend::ConsulBackend(const std::string& host, int port) : mHost(host), mPort(port),
    mConsul(host + ":" + std::to_string(port)), mStorage(mConsul)
{
}

ConsulBackend::~ConsulBackend()
{
}

void ConsulBackend::setPrefix(const std::string& path)
{
  mPrefix = trimLeadingSlash(path);
}

/// Prefix the prefix to the path
auto ConsulBackend::addPrefix(const std::string& path) -> std::string
{
  return mPrefix + path;
}

/// Replace separators in the path
auto ConsulBackend::replaceSeparator(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), '/', getSeparator());
  return p;
}

void ConsulBackend::putString(const std::string& path, const std::string& value)
{
  mStorage.put(addPrefix(replaceSeparator(trimLeadingSlash(path))), value);
}

auto ConsulBackend::getString(const std::string& path) -> Optional<std::string>
{
  auto item = mStorage.item(addPrefix(replaceSeparator(trimLeadingSlash(path))),
      ppconsul::keywords::consistency = ppconsul::Consistency::Stale);
  if (item.valid()) {
    return std::move(item.value);
  } else {
    return {};
  }
}

auto ConsulBackend::getItems(const std::string& requestKey) -> std::vector<ppconsul::kv::KeyValue>
{
  return mStorage.items(requestKey, ppconsul::keywords::consistency = ppconsul::Consistency::Stale);
}

auto ConsulBackend::getRecursive(const std::string& path) -> Tree::Node
{
  auto requestKey = addPrefix(replaceSeparator(trimLeadingSlash(path)));
  auto items = getItems(requestKey);
  std::vector<std::pair<std::string, Tree::Leaf>> keyValuePairs(items.size());
  for (const auto& item : items) {
    keyValuePairs.emplace_back(stripRequestKey(requestKey, item.key), std::move(item.value));
  }
  return Tree::keyValuesToTree(keyValuePairs);
}

auto ConsulBackend::getRecursiveMap(const std::string& path) -> KeyValueMap
{
  auto requestKey = addPrefix(replaceSeparator(trimLeadingSlash(path)));
  auto items = getItems(requestKey);
  KeyValueMap map;
  for (const auto& item : items) {
    map[stripRequestKey(requestKey, item.key)] = std::move(item.value);
  }
  return map;
}

} // namespace backends
} // namespace configuration
} // namespace o2

