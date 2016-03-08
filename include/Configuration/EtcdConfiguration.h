/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten, CERN

#ifndef SRC_ETCDCONFIGURATION_H_
#define SRC_ETCDCONFIGURATION_H_

#include <Configuration/ConfigurationInterface.h>

class EtcdConfiguration: public ConfigurationInterface
{
  public:
    EtcdConfiguration(std::string host, int port);
    virtual ~EtcdConfiguration();

    virtual void putString(std::string path, std::string value);
    virtual std::string getString(std::string path);

  private:
    std::string host;
    int port;
};

#endif /* SRC_ETCDCONFIGURATION_H_ */
