/// \file IniBackend.h
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef O2_CONFIGURATION_INIBACKEND_H_
#define O2_CONFIGURATION_INIBACKEND_H_

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
class IniBackend final : public BackendBase
{
  public:
    IniBackend(const std::string& filePath);
    virtual ~IniBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual boost::optional<std::string> getString(const std::string& path) override;

  private:
    std::string mFilePath;
    boost::property_tree::ptree mPropertyTree;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_INIBACKEND_H_
