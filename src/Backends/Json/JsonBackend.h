/// \file JsonBackend.h
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

class JsonBackend final : public BackendBase
{
  public:
    JsonBackend(const std::string& filePath);
    virtual ~JsonBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual boost::property_tree::ptree getRecursive(const std::string& path) override;
    virtual auto getRecursiveMap(const std::string& path) -> KeyValueMap override;
  private:
    std::string mFilePath;
    boost::property_tree::ptree mTree;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_JSONBACKEND_H_
