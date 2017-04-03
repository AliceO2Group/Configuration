/// \file FileBackend.h
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDS_FILE_FILEBACKEND_H_
#define SRC_FILECONFIGURATION_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "../BackendBase.h"

namespace AliceO2
{
namespace Configuration
{
namespace Backends
{

/// Backend for .ini files
class FileBackend final : public BackendBase
{
  public:
    FileBackend(const std::string& filePath);
    virtual ~FileBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;

  private:
    std::string mFilePath;
    boost::property_tree::ptree mPropertyTree;
};

} // namespace Backends
} // namespace Configuration
} // namespace AliceO2

#endif /* SRC_FILECONFIGURATION_H_ */
