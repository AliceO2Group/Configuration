// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file TestBackend.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include <iostream>
#include "Configuration/ConfigurationFactory.h"
#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
  std::string uri;
  boost::program_options::options_description desc("Tests Configuration backend.");
  desc.add_options()
    ("backend", boost::program_options::value<std::string>(&uri)->required(), "Backend URI")
    ("get-key", boost::program_options::value<std::string>(), "Key to get a value (optional)")
  ;
  
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);


  using namespace o2::configuration;
  std::cout << "Testing backned: " << uri << std::endl;
  auto source = ConfigurationFactory::getConfiguration(uri);
  if (vm.count("get-key")) {
    std::string key = vm["get-key"].as<std::string>();
    std::cout << "Reading from key: " << key << std::endl;
    std::cout << "Value read: " << source->get<std::string>(key) << std::endl;
  }
}
