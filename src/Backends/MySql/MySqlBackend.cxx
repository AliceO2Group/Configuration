/// \file EtcdBackend.cxx
/// \brief Implementation of configuration interface to the etcd distributed key-value store using the v3 API
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "MySqlBackend.h"
#include <sstream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <mysql/mysql.h>

using namespace std::string_literals;


#define MYSQL_USER "root"
#define MYSQL_PASS nullptr
#define MYSQL_UNIX_SOCKET nullptr
#define MYSQL_CLIENT_FLAG 0

#define MYSQL_DATABASE "o2"
#define MYSQL_TABLE "configuration"
#define MYSQL_DATABASE_TABLE MYSQL_DATABASE "." MYSQL_TABLE
#define MYSQL_KEY "kee"
#define MYSQL_KEY_LENGTH "512"
#define MYSQL_VALUE "value"


namespace AliceO2
{
namespace Configuration
{
namespace Backends
{
namespace
{
} // Anonymous namespace

struct MySqlHandle
{
    MySqlHandle()
    {
      handle = mysql_init(nullptr);
      if (handle == nullptr) {
        throw std::runtime_error(mysql_error(handle));
      }
    }

    ~MySqlHandle()
    {
      if (handle != nullptr) {
        mysql_close(handle);
      }
    }

    MYSQL* handle;
};

class MySqlBackendPimpl
{
  public:

    MySqlBackendPimpl(const std::string& host, int port) : handle()
    {
      if (mysql_real_connect(getHandle(), host.c_str(), MYSQL_USER, MYSQL_PASS, MYSQL_DATABASE, port,
          MYSQL_UNIX_SOCKET, MYSQL_CLIENT_FLAG) == 0)
      {
        throw std::runtime_error(mysql_error(getHandle()));
      }
    }

    ~MySqlBackendPimpl()
    {
    }

    MYSQL* getHandle()
    {
      return handle.handle;
    }

  private:

    MySqlHandle handle;
};

MySqlBackend::MySqlBackend(const std::string& host, int port)
    : mPimpl(new MySqlBackendPimpl(host, port))
{

}

void MySqlBackend::putString(const std::string& path, const std::string& value)
{
  auto query = (boost::format("INSERT INTO " MYSQL_DATABASE_TABLE " VALUES('%1%', '%2%')") % path % value).str();
  if (mysql_query(mPimpl->getHandle(), query.c_str())) {
    throw std::runtime_error("MySQL query failed");
  }
}

auto MySqlBackend::getString(const std::string& path) -> Optional<std::string>
{
  auto query = (boost::format("SELECT " MYSQL_VALUE "FROM " MYSQL_TABLE " WHERE " MYSQL_KEY "='%1%'") % path).str();
  if (mysql_query(mPimpl->getHandle(), query.c_str())) {
    throw std::runtime_error("MySQL query failed");
  }

  // TODO read data
  MYSQL_RES* result = mysql_store_result(mPimpl->getHandle());
  if (result == nullptr) {
    throw std::runtime_error("MySQL result == nullptr");
  }

  if (mysql_num_fields(result) != 1) {
    throw std::runtime_error("MySQL returned incorrect number of fields (!=1)");
  }

  MYSQL_ROW row = mysql_fetch_row(result);
  return std::string(row[0]);
}

auto MySqlBackend::getRecursive(const std::string& path) -> Tree::Node
{
  auto query = (boost::format(
      "SELECT " MYSQL_KEY "," MYSQL_VALUE " FROM " MYSQL_TABLE " WHERE " MYSQL_KEY " LIKE '%1%%%'") % path).str();

  if (mysql_query(mPimpl->getHandle(), query.c_str())) {
    throw std::runtime_error("MySQL query failed");
  }

  // TODO read data
  MYSQL_RES* result = mysql_store_result(mPimpl->getHandle());
  if (result == nullptr) {
    throw std::runtime_error("MySQL result == nullptr");
  }

  if (mysql_num_fields(result) != 2) {
    throw std::runtime_error("MySQL returned incorrect number of fields (!=2)");
  }

  std::vector<std::pair<std::string, Tree::Leaf>> keyValuePairs;
  {
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
      keyValuePairs.emplace_back(row[0], row[1]);
    }
  }

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
