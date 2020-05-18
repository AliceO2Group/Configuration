// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file ConfigurationFactory.h
/// \brief Factory for instantiating specialized configuration interfaces
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONFACTORY_H_
#define ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONFACTORY_H_

#include <string>
#include <memory>
#include "Configuration/ConfigurationInterface.h"

namespace o2
{
namespace configuration
{

class ConfigurationFactory
{
  public:
    /// Get a ConfigurationInterface suitable for the given URI
    /// The URI specifies the type of the backend, its location or directory, and possibly port.
    ///
    /// Usage example:
    ///   \snippet test/TestExamples.cxx [Example]
    ///
    /// \param uri The URI
    /// \return A unique_ptr containing a pointer to an interface to the requested back-end
    static std::unique_ptr<ConfigurationInterface> getConfiguration(const std::string& uri);
};

} // Configuration
} // AliceO2

#endif // ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATIONFACTORY_H_
