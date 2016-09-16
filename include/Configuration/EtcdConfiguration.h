/// \file EtcdConfiguration.h
/// \brief Configuration interface to the ETCD distributed key-value store
///
/// \author Pascal Boeschoten, CERN

#ifndef SRC_ETCDCONFIGURATION_H_
#define SRC_ETCDCONFIGURATION_H_

#include <Configuration/ConfigurationInterface.h>
#include <boost/scoped_ptr.hpp>


namespace AliceO2
{
namespace Configuration
{

struct EtcdState;

class EtcdConfiguration: public ConfigurationInterface
{
  public:
    EtcdConfiguration(std::string host, int port);
    virtual ~EtcdConfiguration();

    virtual void putString(std::string path, std::string value);
    virtual std::string getString(std::string path);

  private:
    boost::scoped_ptr<EtcdState> etcdState;
    std::string host;
    int port;
};

}
}

#endif /* SRC_ETCDCONFIGURATION_H_ */
