/// \file EtcdBackend.h
/// \brief Definition of configuration interface to the etcd distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDS_MYSQL_MYSQLBACKEND_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDS_MYSQL_MYSQLBACKEND_H_

#include <string>
#include <memory>
#include "../BackendBase.h"

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{

struct MySqlBackendPimpl;

/// Backend for etcd with the v3 API.
class MySqlBackend final : public BackendBase
{
  public:
    MySqlBackend(const std::string& host, int port);
    ~MySqlBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual Optional<std::string> getString(const std::string& path) override;
    virtual void setPrefix(const std::string& path) override;
    virtual auto getRecursive(const std::string& path) -> Tree::Node override;

  private:
    auto addPrefix(const std::string& path) -> std::string;
    std::unique_ptr<MySqlBackendPimpl> mPimpl;
    std::string mPrefix;
};

} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDS_MYSQL_MYSQLBACKEND_H_
