/// \file FileBackend.h
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_SRC_BACKENDS_FILE_FILEBACKEND_H_
#define ALICEO2_CONFIGURATION_SRC_BACKENDS_FILE_FILEBACKEND_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "../BackendBase.h"

namespace o2
{
namespace configuration
{
namespace backends
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

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // ALICEO2_CONFIGURATION_SRC_BACKENDS_FILE_FILEBACKEND_H_
