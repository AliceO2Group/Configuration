/// \file NdbBackend.h
/// \brief Configuration interface to the NDB distributed key-value store
///
/// NOTE: Very work in progress. Non-functional. Is not enabled/compiled yet.
///
/// \author Pascal Boeschoten, CERN

#include "NdbBackend.h"
#include <iostream>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <mysql/storage/ndb/ndbapi/NdbApi.hpp>
#include <mysql/storage/ndb/ndb_types.h>

// Defines for MYSQL
#define O2_CONFIGURATION_MYSQL_DB_NAME "o2_configuration_db"
#define O2_CONFIGURATION_MYSQL_TABLE_NAME "o2_configuration_blob"
#define O2_CONFIGURATION_MYSQL_TABLE_KEY_NAME "ATTR1"
#define O2_CONFIGURATION_MYSQL_TABLE_KEY_LENGTH "255"
#define O2_CONFIGURATION_MYSQL_TABLE_VALUE_NAME "ATTR2"

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
int tableKeyLength()
{
  return boost::lexical_cast<int>(O2_CONFIGURATION_MYSQL_TABLE_KEY_LENGTH);
}

/// Need to prepend 1 or 2 little-endian bytes to indicate length. Yes, seriously.
std::string keyPrependSizeBytes(std::string keyIn)
{
  if (tableKeyLength() >= 65536) {
    throw std::runtime_error("Key too big for 2 prepend bytes");
  }

  uint16_t len = keyIn.length();
  uint8_t len0 = len & 0x00FF;
  std::string keyOut;
  keyOut += len0;
  if (tableKeyLength() >= 256) {
    uint8_t len1 = (len & 0xFF00) >> 8;
    keyOut += len1;
  }
  keyOut += keyIn;
  return keyOut;
}

struct ActiveHookData
{
    char buffer[100 * 1000];
    Uint32 readLength;
};

int myFetchHook(NdbBlob* myBlobHandle, void* arg)
{
  ActiveHookData* ahd = static_cast<ActiveHookData*>(arg);
  ahd->readLength = sizeof(ahd->buffer) - 1;
  return myBlobHandle->readData(ahd->buffer, ahd->readLength);
}
} // Anonymous namespace

struct NdbBackendPimpl
{
    NdbBackendPimpl(std::string mySqlSocket, std::string clusterConnectString)
    {
      ndb_init();

      // Connect to mysql server and cluster
//      std::cout << "mySqlSocket: " << mySqlSocket << std::endl;
//      std::cout << "clsConnStr : " << clusterConnectString << std::endl;
      clusterConnection.reset(new ::Ndb_cluster_connection(clusterConnectString.c_str()));

      // Connect to cluster management server (ndb_mgmd)
      if (clusterConnection->connect(4, 5, 1)) {
        throw std::runtime_error("Cluster management server was not ready within 30 seconds");
      }

      // Optionally connect and wait for the storage nodes (ndbd's)
      if (clusterConnection->wait_until_ready(30, 0) < 0) {
        throw std::runtime_error("Cluster was not ready within 30 seconds");
      }

      // Connect to mysql server
      mySql.reset(new MYSQL());
      if (!mysql_init(mySql.get())) {
        throw std::runtime_error("mysql_init failed");
      }
      if (!mysql_real_connect(mySql.get(), "localhost", "root", "", "", 0, mySqlSocket.c_str(), 0)) {
        throw std::runtime_error("MySQL error");
      }

      // Connect to database via mysql-c
      mysql_query(mySql.get(), "CREATE DATABASE " O2_CONFIGURATION_MYSQL_DB_NAME);
      if (mysql_query(mySql.get(), "USE " O2_CONFIGURATION_MYSQL_DB_NAME) != 0) {
        throw std::runtime_error("MySQL error");
      }

      // Connect to database via NdApi
      myNdb.reset(new ::Ndb(clusterConnection.get(), O2_CONFIGURATION_MYSQL_DB_NAME));
      if (myNdb->init()) {
        throw std::runtime_error(myNdb->getNdbError().message);
      }
    }

    ~NdbBackendPimpl()
    {
      //ndb_end(0);
    }

    std::string mHost;
    int mPort;
    std::string mPrefix;
    std::unique_ptr<::Ndb_cluster_connection> clusterConnection;
    std::unique_ptr<::MYSQL> mySql;
    std::unique_ptr<::Ndb> myNdb;
};

NdbBackend::NdbBackend(const std::string& mySqlSocket, const std::string& clusterConnectString)
    : mPimpl(new NdbBackendPimpl(mySqlSocket, clusterConnectString))
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
  // Create the table.
  // This poses a bit of an overhead on put operations. In the future, the table should be created during cluster setup.
  while (mysql_query(mPimpl->mySql.get(),
    "CREATE TABLE"
      "  " O2_CONFIGURATION_MYSQL_TABLE_NAME
      "    (" O2_CONFIGURATION_MYSQL_TABLE_KEY_NAME " VARCHAR(" O2_CONFIGURATION_MYSQL_TABLE_KEY_LENGTH ") NOT NULL PRIMARY KEY,"
      "     " O2_CONFIGURATION_MYSQL_TABLE_VALUE_NAME " TEXT NOT NULL)"
      "  ENGINE=NDB")) {
    if (mysql_errno(mPimpl->mySql.get()) == ER_TABLE_EXISTS_ERROR) {
//      std::cout << "MySQL Cluster already has table: " O2_CONFIGURATION_MYSQL_TABLE_NAME << std::endl;
      break;
    } else {
      throw std::runtime_error("MySQL error");
    }
  }

  const NdbDictionary::Dictionary* myDict = mPimpl->myNdb->getDictionary();
  const NdbDictionary::Table* myTable = myDict->getTable(O2_CONFIGURATION_MYSQL_TABLE_NAME);

  NdbTransaction* myTransaction = mPimpl->myNdb->startTransaction();
  if (myTransaction == nullptr) {
    throw std::runtime_error(mPimpl->myNdb->getNdbError().message);
  }

  NdbOperation* myNdbOperation = myTransaction->getNdbOperation(myTable);
  if (myNdbOperation == nullptr) {
    throw std::runtime_error(myTransaction->getNdbError().message);
  }

  myNdbOperation->insertTuple();
  myNdbOperation->equal(O2_CONFIGURATION_MYSQL_TABLE_KEY_NAME, path.data(), path.length());

  std::cout << "Putting key:" << path << std::endl;

  NdbBlob* myBlobHandle = myNdbOperation->getBlobHandle(O2_CONFIGURATION_MYSQL_TABLE_VALUE_NAME);
  if (myBlobHandle == nullptr) {
    throw std::runtime_error(myNdbOperation->getNdbError().message);
  }

  std::cout << "WRITE LENGTH: " << value.length() << std::endl;
  myBlobHandle->setValue(value.data(), value.length());
  Uint64 blobLength = -1;
  myBlobHandle->getLength(blobLength);
  std::cout << "BLOB LENGTH: " << blobLength << std::endl;

  if (myTransaction->execute(NdbTransaction::Commit) == -1) {
    if (myTransaction->getNdbError().code != 630) {
      // 630 is error code for 'Tuple already existed when attempting to insert'
      throw std::runtime_error(myTransaction->getNdbError().message);
    }
  }
  myTransaction->close();
}

auto NdbBackend::getString(const std::string& path) -> Optional<std::string>
{
  std::string value;
  const NdbDictionary::Dictionary* myDict = mPimpl->myNdb->getDictionary();
  const NdbDictionary::Table* myTable = myDict->getTable(O2_CONFIGURATION_MYSQL_TABLE_NAME);

  NdbTransaction* myTransaction = mPimpl->myNdb->startTransaction();
  if (myTransaction == nullptr) {
    throw std::runtime_error(mPimpl->myNdb->getNdbError().message);
  }

  NdbOperation* myNdbOperation = myTransaction->getNdbOperation(myTable);
  if (myNdbOperation == nullptr) {
    throw std::runtime_error(myTransaction->getNdbError().message);
  }

  std::string sqlKey = keyPrependSizeBytes(path);
  myNdbOperation->readTuple(NdbOperation::LM_Read);
  myNdbOperation->equal(O2_CONFIGURATION_MYSQL_TABLE_KEY_NAME, sqlKey.data(), sqlKey.length());

  NdbBlob* myBlobHandle = myNdbOperation->getBlobHandle(O2_CONFIGURATION_MYSQL_TABLE_VALUE_NAME);
  if (myBlobHandle == nullptr) {
    throw std::runtime_error(myNdbOperation->getNdbError().message);
  }

  ActiveHookData ahd;
  if (myBlobHandle->setActiveHook(myFetchHook, &ahd) == -1) {
    throw std::runtime_error(myBlobHandle->getNdbError().message);
  }

  if (myTransaction->execute(NdbTransaction::Commit) == -1) {
    throw std::runtime_error(myTransaction->getNdbError().message);
  }

  int isNull = 0;
  if (myBlobHandle->getDefined(isNull)){
    throw std::runtime_error("Blob not found");
  }

  ahd.buffer[ahd.readLength] = '\0';
  value = std::string(ahd.buffer, ahd.readLength);

  std::cout << "BLOB RETRIEVED -- readLength:" << ahd.readLength << " path:" << path << " value:" << value << std::endl;
  mPimpl->myNdb->closeTransaction(myTransaction);
  std::cout << "CLOSED TRANSACTION" << std::endl;

  return value;
}

auto NdbBackend::getRecursive(const std::string& path) -> Tree::Node
{
  std::ostringstream oss;
  oss << "SELECT * FROM " O2_CONFIGURATION_MYSQL_TABLE_NAME
    << " WHERE " O2_CONFIGURATION_MYSQL_TABLE_KEY_NAME
    << " LIKE '" << path << "%'";

  if (mysql_query(mPimpl->mySql.get(), oss.str().c_str())) {
    throw std::runtime_error("MySQL query failed");
  }

  // TODO read data
  ::MYSQL_RES* result = mysql_store_result(mPimpl->mySql.get());
  if (result == nullptr) {
    throw std::runtime_error("MySQL result == NULL");
  }

  if (mysql_num_fields(result) != 2) {
    throw std::runtime_error("MySQL returned incorrect number of fields (!=2)");
  }

//  std::vector<std::string, std::string> keyValues;
  std::vector<std::pair<std::string, Tree::Leaf>> keyValuePairs;
  {
    ::MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
      keyValuePairs.emplace_back(row[0], row[1]);
    }
  }

  Tree::keyValuesToTree(keyValuePairs);
}

} // namespace Ndb
} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

