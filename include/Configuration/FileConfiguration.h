/// \file FileConfiguration.h
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef SRC_FILECONFIGURATION_H_
#define SRC_FILECONFIGURATION_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "Configuration/ConfigurationInterface.h"

namespace AliceO2
{
namespace Configuration
{

class FileConfiguration final : public ConfigurationInterface
{
  public:
    FileConfiguration(std::string filePath);
    virtual ~FileConfiguration();

    virtual void putString(const std::string& path, const std::string& value) override;
    virtual Optional<std::string> getString(const std::string& path) override;

  private:
    std::string filePath;
    boost::property_tree::ptree pt;
};

} // namespace Configuration
} // namespace AliceO2

#endif /* SRC_FILECONFIGURATION_H_ */
