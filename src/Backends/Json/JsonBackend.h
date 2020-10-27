// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

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
    /// \param file A file path to JSON file or JSON data
    JsonBackend(const std::string& file);

    /// Default destructor
    virtual ~JsonBackend() = default;
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual void putRecursive(const std::string& path, const boost::property_tree::ptree& tree) override;
    virtual boost::optional<std::string> getString(const std::string& path) override;
    virtual boost::property_tree::ptree getRecursive(const std::string& path) override;
    virtual KeyValueMap getRecursiveMap(const std::string& path) override;
    void readJsonFile(bool isStream = false);
  private:
    /// Parsed JSON file
    boost::property_tree::ptree mTree;
    std::string mPath;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_BACKENDS_JSONBACKEND_H_
