/// \file Copy.cxx
/// \brief Command-line utility for copying values from a configuration backend
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "Configuration/ConfigurationFactory.h"
#include "Configuration/Tree.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  bool help;
  std::string sourceUri;
  std::string destinationUri;
  auto example = R"(Example: 
configuration-copy --source="etcd-v3://host1:2379/stuff/" --dest="consul://host2:8500/some/dirs/new/stuff/)";

  {
    auto optionsDescription = po::options_description("Options");
    optionsDescription.add_options()
        ("help", po::bool_switch(&help), "Print help")
        ("source,s", po::value<std::string>(&sourceUri)->required(), "Source server URI")
        ("dest,d", po::value<std::string>(&destinationUri)->required(), "Destination server URI");

    try {
      auto map = po::variables_map();
      po::store(po::parse_command_line(argc, argv, optionsDescription), map);
      po::notify(map);
      if (help) {
        std::cout << optionsDescription << example << '\n';
        return 0;
      }
    }
    catch (const po::error& e) {
      std::cout << "Error: " << e.what() << '\n' << optionsDescription << example << '\n';
      return 0;
    }
  }

  using namespace AliceO2::Configuration;
  auto source = ConfigurationFactory::getConfiguration(sourceUri);
  auto destination = ConfigurationFactory::getConfiguration(destinationUri);
  auto keyValues = Tree::treeToKeyValues(source->getRecursive("/"));

  for (const auto& kv : keyValues) {
    Visitor::apply(kv.second,
        [&](const std::string& value) { destination->put<std::string>(kv.first, value); },
        [&](int value) { destination->put<int>(kv.first, value); },
        [&](bool value) { destination->put<int>(kv.first, int(value)); },
        [&](double value) { destination->put<double>(kv.first, value); });
  }

  return 0;
}

