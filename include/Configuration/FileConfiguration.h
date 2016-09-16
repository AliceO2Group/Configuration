/// \file FileConfiguration.h
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten, CERN

#ifndef SRC_FILECONFIGURATION_H_
#define SRC_FILECONFIGURATION_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <Configuration/ConfigurationInterface.h>

namespace AliceO2
{
namespace Configuration
{

class FileConfiguration : public ConfigurationInterface
{
  public:
    FileConfiguration(std::string filePath);

    virtual ~FileConfiguration();

    virtual void putString(std::string path, std::string value);

    virtual std::string getString(std::string path);

  private:
    std::string filePath;
    boost::property_tree::ptree pt;
};

}
}

#endif /* SRC_FILECONFIGURATION_H_ */
