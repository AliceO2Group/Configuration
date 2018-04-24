/// \file JsonBackend.cxx
/// \brief Definition of configuration interface for JSON files
///
/// \author Pascal Boeschoten, CERN

#ifndef ALICEO2_CONFIGURATION_SRC_JSONBACKEND_H_
#define ALICEO2_CONFIGURATION_SRC_JSONBACKENDH_

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "../BackendBase.h"

namespace o2
{
namespace configuration
{
namespace backends
{

/// Backend for .json files
class JsonBackend final : public BackendBase
{
  public:
    JsonBackend(const std::string& filePath);
    virtual ~JsonBackend();
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual auto getString(const std::string& path) -> Optional<std::string> override;
    virtual void setPrefix(const std::string& path) override;
    virtual auto getRecursive(const std::string& path) -> tree::Node override;
    virtual auto getRecursiveMap(const std::string& path) -> KeyValueMap override;

  private:
    std::string mFilePath;
    tree::Node mRootNode;
    tree::Node mCurrentNode;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif /* ALICEO2_CONFIGURATION_SRC_JSONBACKEND_H_ */
