/// \file NdbBackend.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten, CERN

#include "NdbBackend.h"

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace Ndb
{
namespace
{
} // Anonymous namespace

NdbBackend::NdbBackend(const std::string& host, int port) : mHost(host), mPort(port)
{
}

NdbBackend::~NdbBackend()
{
}

void NdbBackend::setPrefix(const std::string& path)
{
  mPrefix = path;
}

void NdbBackend::putString(const std::string& path, const std::string& value)
{
}

auto NdbBackend::getString(const std::string& path) -> Optional<std::string>
{
  return {};
}

auto NdbBackend::getRecursive(const std::string& path) -> Tree::Node
{
  return {};
}

} // namespace Ndb
} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

