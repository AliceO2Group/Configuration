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
  assert(response.find(requestKey) == 0);
  return response.substr(requestKey.length() + 1);
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
  auto item = mStorage.item(replaceDefaultWithSlash(addPrefix(path)),
      ppconsul::kw::consistency = ppconsul::Consistency::Stale);
  if (item.valid()) {
    return std::move(item.value);
  } else {
    return {};
  }
}

boost::property_tree::ptree ConsulBackend::getRecursive(const std::string& path)
{
  auto requestKey = replaceDefaultWithSlash(addPrefix(path)) + "[]";
  auto items = mStorage.items(requestKey, ppconsul::kw::consistency = ppconsul::Consistency::Stale);
  bool isArray = true;
  if (items.size() == 0) {
    isArray = false;
    requestKey = requestKey.substr(0, requestKey.size() - 2);
    items = mStorage.items(requestKey, ppconsul::kw::consistency = ppconsul::Consistency::Stale);
  }
  boost::property_tree::ptree tree;
  for (const auto& item : items) {
    if (!item.value.empty()) {
      tree.put(replaceSlashWithDefault(stripRequestKey(requestKey, item.key)), std::move(item.value));
    }
  }

  using boost::property_tree::ptree;
  std::function<void(ptree&, bool)> parse = [&](ptree& node, bool replace) {
    if (replace) {
      ptree children;
      for (auto& it: node) {
        children.push_back(std::make_pair("", it.second));
      }
      node.swap(children);
    }
    for (ptree::iterator it = node.begin(); it != node.end(); it++) {
      if (it->first.back() == ']') {
        parse(it->second, true);
        node.insert(it, make_pair(it->first.substr(0, it->first.length() - 2), it->second));
        node.erase(it);
      } else {
        parse(it->second, false);
      }
    }
  };
  parse(tree, isArray);
  return tree;
}

KeyValueMap ConsulBackend::getRecursiveMap(const std::string& path)
{
  auto requestKey = replaceDefaultWithSlash(addPrefix(path));
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

