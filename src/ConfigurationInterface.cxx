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

#include "Configuration/ConfigurationInterface.h"
#include <boost/lexical_cast.hpp>

namespace o2 {
namespace configuration {

ConfigurationInterface::~ConfigurationInterface() {}

// Template specializations of the convenience interface methods put/get

template <>
void ConfigurationInterface::put(const std::string &path,
                                 const std::string &value) {
  putString(path, value);
}

template <>
void ConfigurationInterface::put(const std::string &path, const int &value) {
  putString(path, boost::lexical_cast<std::string>(value));
}

template <>
void ConfigurationInterface::put(const std::string &path, const double &value) {
  putString(path, boost::lexical_cast<std::string>(value));
}

void ConfigurationInterface::putRecursive(
    const std::string & /* path*/,
    const boost::property_tree::ptree & /* tree*/) {
  throw std::runtime_error(
      "Recursive put not supported in the selected backend");
}

template <> std::string ConfigurationInterface::get(const std::string &path) {
  auto optional = getString(path);
  return (optional != boost::none)
             ? optional.value()
             : throw std::runtime_error("Could not find: " + path);
}

template <> int ConfigurationInterface::get(const std::string &path) {
  auto optional = getString(path);
  return (optional != boost::none)
             ? std::stoi(optional.value())
             : throw std::runtime_error("Could not find: " + path);
}

template <> double ConfigurationInterface::get(const std::string &path) {
  auto optional = getString(path);
  return (optional != boost::none)
             ? std::stod(optional.value())
             : throw std::runtime_error("Could not find: " + path);
}

template <> bool ConfigurationInterface::get(const std::string &path) {
  auto optional = getString(path);
  if (optional != boost::none) {
    bool boolValue;
    std::istringstream(optional.value()) >> std::boolalpha >> boolValue;
    return boolValue;
  }

  throw std::runtime_error("Could not find: " + path);
}

template <>
auto ConfigurationInterface::get(const std::string &path,
                                 const std::string &defaultValue)
    -> std::string {
  auto optional = getString(path);
  return (optional != boost::none) ? optional.value() : defaultValue;
}

template <>
auto ConfigurationInterface::get(const std::string &path,
                                 const int &defaultValue) -> int {
  auto optional = getString(path);
  return (optional != boost::none) ? stoi(optional.value()) : defaultValue;
}

template <>
auto ConfigurationInterface::get(const std::string &path,
                                 const double &defaultValue) -> double {
  auto optional = getString(path);
  return (optional != boost::none) ? std::stod(optional.value()) : defaultValue;
}

template <>
auto ConfigurationInterface::get(const std::string &path,
                                 const bool &defaultValue) -> bool {
  auto optional = getString(path);
  if (optional != boost::none) {
    bool boolValue;
    std::istringstream(optional.value()) >> std::boolalpha >> boolValue;
    return boolValue;
  }

  return defaultValue;
}

} // namespace configuration
} // namespace o2
