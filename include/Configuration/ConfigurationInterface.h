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

/// \file ConfigurationInterface.h
/// \brief Interface for putting and getting configuration parameters.
///
/// \author Pascal Boeschoten, CERN
/// \author Adam Wegrzynek, CERN

#ifndef O2_CONFIGURATION_CONFIGURATIONINTERFACE_H_
#define O2_CONFIGURATION_CONFIGURATIONINTERFACE_H_

#include <string>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

namespace o2
{
namespace configuration
{

using KeyValueMap = std::unordered_map<std::string, std::string>;

/// \brief Interface for configuration back ends.
///
/// Interface for configuration back-ends, to put and get configuration parameters.
///
/// To maintain runtime polymorphism, while also having a convenient template-like interface,
/// the implementations of put/get operations are in the virtual methods.
class ConfigurationInterface
{
  public:
    virtual ~ConfigurationInterface();

    /// Puts a string into the configuration.
    /// \param path The path of the value
    /// \param value The value to put
    virtual void putString(const std::string& path, const std::string& value) = 0;

    /// Retrieves a string value from the configuration.
    /// \param path The path of the value
    /// \return The retrieved value
    virtual boost::optional<std::string> getString(const std::string& path) = 0;

    /// Template convenience interface for put operations. Redirects to the appropriate virtual method.
    /// \param T The type of the value. Supported types are "std::string", "int" and "double"
    /// \param path The path of the value
    /// \param value The value to put
    template<typename T>
    void put(const std::string& path, const T& value);

    /// Recursively puts values to the given path
    /// \param path The path to the destination
    /// \param tree Tree-like data structure
    virtual void putRecursive(const std::string& path, const boost::property_tree::ptree& tree);

    /// Template convenience interface for get operations.
    /// \param path The path of the value
    /// \return The retrieved value.
    /// \throw std::runtime_error when value does not exist
    template<typename T>
    T get(const std::string& path);

    /// Template convenience interface for get operations with a  default value
    /// \param path The path of the value
    /// \param defaultValue default value which is returned when requested key does not exist
    template<typename T>
    T get(const std::string& path, const T& defaultValue);

    /// Sets a prefix
    /// After this call, all paths given to this object will be prefixed with this.
    /// The implementation of this is very backend-dependent and it may not be a trivial call.
    /// \param prefix The prefix path
    virtual void setPrefix(const std::string& prefix) = 0;

    /// Gets key-values recursively from the given path
    /// \param path The path of the values to get
    /// \return A map containing the key-values
    virtual KeyValueMap getRecursiveMap(const std::string& path = {}) = 0;

    /// Provides subtree from given path
    /// \param path The path to the subtree
    /// \return Subtree
    virtual boost::property_tree::ptree getRecursive(const std::string& path = {}) = 0;
};

} // namespace configuration
} // namespace o2

#endif /* O2_CONFIGURATION_CONFIGURATIONINTERFACE_H_ */
