// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file IniBackend.h
/// \brief Configuration interface for INI file backend
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten, CERN

#ifndef O2_CONFIGURATION_BACKENDS_INIBACKEND_H_
#define O2_CONFIGURATION_BACKENDS_INIBACKEND_H_

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
    /// Read and parse INI file
    IniBackend(const std::string& file, bool isStream = false);

    /// Default destructor
    virtual ~IniBackend() = default;
    virtual void putString(const std::string& path, const std::string& value) override;
    virtual boost::optional<std::string> getString(const std::string& path) override;
    virtual boost::property_tree::ptree getRecursive(const std::string&) override;
  private:
    /// Parsed INI file
    boost::property_tree::ptree mPropertyTree;
};

} // namespace backends
} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_BACKENDS_INIBACKEND_H_
