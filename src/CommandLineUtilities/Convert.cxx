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

///
/// \file Convert.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Configuration/ConfigurationFactory.h"
#include "../Backends/Json/JsonBackend.h"
#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
  std::string sourceUri, destinationUri;
  boost::program_options::options_description desc("Copies values from source to destination.");
  desc.add_options()
    ("src", boost::program_options::value<std::string>(&sourceUri)->required(), "Source URI")
    ("dest", boost::program_options::value<std::string>(&destinationUri)->required(), "Destination URI")
  ;

  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);


  using namespace o2::configuration;
  auto source = ConfigurationFactory::getConfiguration(sourceUri);
  auto values = source->getRecursive("");

  // Workaround for writing JSON
  if (destinationUri.substr(0,4) == "json") {
    auto destination = new backends::JsonBackend("json://");
    destination->putRecursive(destinationUri.substr(5), values);
    return 0;
  }

  auto destination = ConfigurationFactory::getConfiguration(destinationUri);
  destination->putRecursive("", values);
}
