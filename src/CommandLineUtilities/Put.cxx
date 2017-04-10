/// \file Put.cxx
/// \brief Command-line utility for putting values to a configuration backend
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "Configuration/ConfigurationFactory.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  bool help;
  std::string serverUri;
  std::string key;
  std::string value;

  {
    auto optionsDescription = po::options_description("Options");
    optionsDescription.add_options()
        ("help", po::bool_switch(&help), "Print help")
        ("uri", po::value<std::string>(&serverUri)->required(), "Server URI")
        ("key,k", po::value<std::string>(&key)->required(), "Key to put to")
        ("value,v", po::value<std::string>(&value)->required(), "Value to put");

    try {
      auto map = po::variables_map();
      po::store(po::parse_command_line(argc, argv, optionsDescription), map);
      po::notify(map);
      if (help) {
        std::cout << optionsDescription << '\n';
        return 0;
      }
    }
    catch (const po::error& e) {
      std::cout << "Error: " << e.what() << '\n' << optionsDescription << '\n';
      return 0;
    }
  }

  auto configuration = AliceO2::Configuration::ConfigurationFactory::getConfiguration(serverUri);
  configuration->putString(key, value);

  return 0;
}

