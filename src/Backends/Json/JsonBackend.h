/// \file JsonBackend.h
/// \brief Read JSON file into ptree data structure
///
/// \author Adam Wegrzynek, CERN

#ifndef O2_CONFIGURATION_JSONBACKEND_H_
#define O2_CONFIGURATION_JSONBACKEND_H_

#include "../BackendBase.h"
#include <string>
#include <boost/property_tree/ptree.hpp>

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
    virtual boost::optional<std::string> getString(const std::string& path) override;
    virtual boost::property_tree::ptree getRecursive(const std::string& path) override;
    virtual KeyValueMap getRecursiveMap(const std::string& path) override;
  private:
    std::string mFilePath;
    boost::property_tree::ptree mTree;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_JSONBACKEND_H_
