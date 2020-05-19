// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file JsonBackend.cxx
/// \brief Configuration interface for files. Port of Configuration.h & Configuration.cxx
///

#include "StringBackend.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>
#include <stdexcept>

namespace o2
{
namespace configuration
{
namespace backends
{

StringBackend::StringBackend(const std::string& s)
{
  auto cfgStr = boost::trim_copy_if(s, boost::is_any_of(" \n\t"));
  if (cfgStr.empty()) {
    throw std::runtime_error("string cfg is empty");
  }

  std::vector<std::string> tokens;
  boost::split(tokens, s, boost::is_any_of(";"));

  for (auto& token : tokens) {
    const auto equals_idx = token.find_first_of('=');
    if (std::string::npos != equals_idx) {
      mTree.put(boost::trim_copy(token.substr(0, equals_idx)),
                boost::trim_copy(token.substr(equals_idx + 1)));
    } else {
      throw std::runtime_error("Not a key value pair" + token);
    }
  }
}

void StringBackend::putString(const std::string&, const std::string&)
{
  throw std::runtime_error("String backend does not support putting values");
}

void StringBackend::putRecursive(const std::string&,
                                 const boost::property_tree::ptree&)
{
  throw std::runtime_error("String backend does not support putting values");
}

boost::optional<std::string> StringBackend::getString(const std::string& path)
{
  // To use a custom separator instead of the default '.', we need to construct the path_type object explicitly
  return mTree.get_optional<std::string>(decltype(mTree)::path_type(addPrefix(path), getSeparator()));
}

boost::property_tree::ptree
  StringBackend::getRecursive(const std::string& path)
{
  return mTree.get_child(decltype(mTree)::path_type(addPrefix(path), getSeparator()));
}

KeyValueMap StringBackend::getRecursiveMap(const std::string& path)
{
  KeyValueMap map;
  auto subTree = mTree.get_child(decltype(mTree)::path_type(addPrefix(path), getSeparator()));

  // define lambda to recursively interate tree
  using boost::property_tree::ptree;
  std::function<void(const ptree&, std::string)> parse = [&](const ptree& node, std::string key) {
    map[key] = std::move(node.data());
    key = key.empty() ? "" : key + getSeparator();
    for (auto const& it : node) {
      parse(it.second, key + it.first);
    }
  };

  parse(subTree, std::string());
  return map;
}

} // namespace backends
} // namespace configuration
} // namespace o2
