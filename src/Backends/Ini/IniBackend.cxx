// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file IniBackend.cxx
/// \brief Configuration interface for files. Port of Configuration.h &
/// Configuration.cxx
///
/// \author Sylvain Chapeland, CERN
/// \author Pascal Boeschoten, CERN

#include "IniBackend.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <vector>

namespace o2
{
namespace configuration
{
namespace backends
{

/// Load the configuration from given path
/// \param path  Path to configuration data.
///              Example: file:/configDir/example.cfg
///              Accepted prefix:
///                 file:     for a file accessible from the filesystem
///              Accepted suffix, to define file format (TODO: separate doc for
///              file formats):
///                 .ini, .cfg    see example.cfg
/// \exception   Throws a <std::string> exception on error.
void loadConfigFile(const std::string& file, boost::property_tree::ptree& pt, bool isStream)
{
  if (file.length() == 0) {
    throw std::runtime_error("Invalid argument");
  }
  try {
    if (isStream) {
      std::istringstream ss;
      ss.str(file);
      boost::property_tree::ini_parser::read_ini(ss, pt);
    } else {
      boost::property_tree::ini_parser::read_ini(file, pt);
    }
  } catch (const boost::property_tree::ini_parser::ini_parser_error& perr) {
    std::stringstream ss;
    if (perr.line()) {
      ss << perr.message() << " in " << perr.filename() << " line "
         << perr.line();
    } else {
      ss << perr.message() << " " << perr.filename();
    }
    throw std::runtime_error(ss.str());
  }
  return;
}

IniBackend::IniBackend(const std::string& file, bool isStream)
{
  loadConfigFile(file, mPropertyTree, isStream);
}

void IniBackend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("IniBackend does not support putting values");
}

boost::optional<std::string> IniBackend::getString(const std::string& path)
{
  // To use a custom separator instead of the default '.', we need to construct
  // the path_type object explicitly
  return mPropertyTree.get_optional<std::string>(
    decltype(mPropertyTree)::path_type(addPrefix(path), getSeparator()));
}

boost::property_tree::ptree IniBackend::getRecursive(const std::string& path)
{
  return mPropertyTree.get_child(
    decltype(mPropertyTree)::path_type(addPrefix(path), getSeparator()));
}

} // namespace backends
} // namespace configuration
} // namespace o2
