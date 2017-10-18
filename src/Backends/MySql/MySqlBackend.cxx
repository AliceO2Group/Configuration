/// \file EtcdBackend.cxx
/// \brief Implementation of configuration interface to the etcd distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "MySqlBackend.h"
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace std::string_literals;

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace
{
} // Anonymous namespace

struct MySqlBackendPimpl
{
};

MySqlBackend::MySqlBackend(const std::string& host, int port)
    : mPimpl(new MySqlBackendPimpl())
{
}

void MySqlBackend::putString(const std::string& path, const std::string& value)
{
}

auto MySqlBackend::getString(const std::string& path) -> Optional<std::string>
{
  return {};
}

auto MySqlBackend::getRecursive(const std::string& path) -> Tree::Node
{
  std::vector<std::pair<std::string, Tree::Leaf>> keyValuePairs;
  return Tree::keyValuesToTree(keyValuePairs);
}

void MySqlBackend::setPrefix(const std::string& path)
{
  mPrefix = path;
}

/// Prefix the prefix to the path
auto MySqlBackend::addPrefix(const std::string& path) -> std::string
{
  return mPrefix + path;
}

/// Replace separators in the path
auto MySqlBackend::replaceSeparator(const std::string& path) -> std::string
{
  auto p = path;
  std::replace(p.begin(), p.end(), '/', getSeparator());
  return p;
}

/// Make the URI string for the channel
auto MySqlBackend::makeChannelString(const std::string& host, int port) -> std::string
{
  return host + ':' + std::to_string(port);
}

} // namespace Backends
} // namespace Configuration
} // namespace AliceO2
