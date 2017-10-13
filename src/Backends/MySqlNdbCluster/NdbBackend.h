/// \file NdbBackend.h
/// \brief Configuration interface to the NDB key-value store
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef SRC_BACKENDS_CONSUL_CONSULBACKEND_H_
#define SRC_BACKENDS_CONSUL_CONSULBACKEND_H_

#include "../BackendBase.h"
#include <string>
#include <boost/scoped_ptr.hpp>

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace Ndb
{

struct NdbBackendPimpl;

/// Backend for Ndb
class NdbBackend final : public BackendBase
{
  public:
    NdbBackend(const std::string& mySqlSocket, const std::string& clusterConnectString);
    virtual ~NdbBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual auto getRecursive(const std::string& path) -> Tree::Node override;

  private:
    boost::scoped_ptr<NdbBackendPimpl> mPimpl;
    std::string mPrefix;
};

} // namespace Ndb
} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

#endif /* SRC_BACKENDS_CONSUL_CONSULBACKEND_H_ */
