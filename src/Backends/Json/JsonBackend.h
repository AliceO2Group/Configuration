/// \file Json2Backend.h
/// \brief Read JSON file into ptree data structure
///
/// \author Adam Wegrzynek, CERN

#ifndef O2_CONFIGURATION_JSONBACKEND_H_
#define O2_CONFIGURATION_JSONBACKEND_H_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "../BackendBase.h"

namespace o2
{
namespace configuration
{
namespace backends
{

class Json2Backend final : public BackendBase
{
  public:
    Json2Backend(const std::string& filePath);
    virtual ~Json2Backend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual boost::property_tree::ptree getSubTree(const std::string& path) override;
  private:
    std::string mFilePath;
    boost::property_tree::ptree mTree;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_JSONBACKEND_H_
