// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file BackendBase.h
/// \brief Base class for ConfigurationInterface backend implementations
///
/// \author Pascal Boeschoten, CERN
/// \author Adam Wegrzynek, CERN

#ifndef O2_CONFIGURATION_BACKENDBASE_H_
#define O2_CONFIGURATION_BACKENDBASE_H_

#include <boost/core/noncopyable.hpp>
#include "Configuration/ConfigurationInterface.h"

namespace o2 {
namespace configuration {

/// Base class for backend implementations. Not it is non-copyable by inheriting form boost::noncopyable
class BackendBase: public ConfigurationInterface, public boost::noncopyable
{
  public:
    /// Path separator getter
    char getSeparator() const
    {
      return DEFAULT_SEPARATOR;
    }

    /// When a backend does not support getRecursiveMap an error is thrown
    virtual boost::property_tree::ptree getRecursive(const std::string&) override
    {
      throw std::runtime_error("getRecursive() unsupported by backend");
    }

    /// When a backend does not support getRecursiveMap an error is thrown
    virtual KeyValueMap getRecursiveMap(const std::string&) override
    {
      throw std::runtime_error("getRecursiveMap() unsupported by backend");
    }

    /// Sets path prefix
    /// \param A path prefix
    virtual void setPrefix(const std::string& prefix) override
    {
      mPrefix = prefix + getSeparator();
    }

  protected:
    /// Prepends path with prefix
    /// \param path A path
    /// \return Provided path with prepended prefix
    std::string addPrefix(const std::string& path)
    {
      return mPrefix + path;
    }

  private:
    /// Default separator for keys/paths
    static constexpr char DEFAULT_SEPARATOR = '.';

    /// Get path prefix
    std::string mPrefix;
};

} // namespace configuration
} // namespace o2

#endif // O2_CONFIGURATION_BACKENDBASE_H_
