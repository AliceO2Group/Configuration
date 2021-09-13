// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file StringBackend.h
/// \brief Interpret a key=value;key2=value2... etc string
///

#ifndef O2_CONFIGURATION_BACKENDS_STRINGBACKEND_H_
#define O2_CONFIGURATION_BACKENDS_STRINGBACKEND_H_

#include "../BackendBase.h"
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace o2
{
namespace configuration
{
namespace backends
{

class StringBackend final : public BackendBase
{
 public:
  /// Interprets a string as key value pairs.
  /// \param s the key=value;key2=value2 string to be used as configuration
  StringBackend(const std::string& s);

  /// Default destructor
  virtual ~StringBackend() = default;
  virtual void putString(const std::string& path,
                         const std::string& value) override;
  virtual boost::optional<std::string>
    getString(const std::string& path) override;
  virtual boost::property_tree::ptree
    getRecursive(const std::string& path) override;
  virtual KeyValueMap getRecursiveMap(const std::string& path) override;

 private:
  boost::property_tree::ptree mTree;
  std::string mPath;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_BACKENDS_STRINGBACKEND_H_
