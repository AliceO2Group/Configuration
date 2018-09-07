/// \file JsonBackend.h
/// \brief Read JSON file into ptree data structure
///
/// \author Adam Wegrzynek, CERN

#ifndef O2_CONFIGURATION_BACKENDS_JSONBACKEND_H_
#define O2_CONFIGURATION_BACKENDS_JSONBACKEND_H_

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
    /// Opens and parses JSON file
    /// \param filePath A file path to JSON file
    JsonBackend(const std::string& filePath);

    /// Default destructor
    virtual ~JsonBackend() = default;
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual void putRecursive(const std::string& path, const boost::property_tree::ptree& tree) override;
    virtual boost::optional<std::string> getString(const std::string& path) override;
    virtual boost::property_tree::ptree getRecursive(const std::string& path) override;
    virtual KeyValueMap getRecursiveMap(const std::string& path) override;
  private:
    /// Parsed JSON file
    boost::property_tree::ptree mTree;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_BACKENDS_JSONBACKEND_H_
