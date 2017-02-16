/// \file NdbBackend.h
/// \brief Configuration interface to the Ndb key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef SRC_BACKENDS_CONSUL_CONSULBACKEND_H_
#define SRC_BACKENDS_CONSUL_CONSULBACKEND_H_

#include "../BackendBase.h"
#include <string>

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace Ndb
{

/// Backend for Ndb
class NdbBackend final : public BackendBase
{
  public:
    NdbBackend(const std::string& host, int port);
    virtual ~NdbBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual auto getRecursive(const std::string& path) -> Tree::Node override;

  private:
    std::string mHost;
    int mPort;
    std::string mPrefix;
};

} // namespace Ndb
} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

#endif /* SRC_BACKENDS_CONSUL_CONSULBACKEND_H_ */
