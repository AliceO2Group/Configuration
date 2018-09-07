///
/// \file Convert.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Configuration/ConfigurationFactory.h"
#include <boost/program_options.hpp>

int main(int argc, char *argv[]) {
  std::string sourceUri, destinationUri;

  boost::program_options::options_description desc("Copies values from source to destination.");
  desc.add_options()
    ("source", boost::program_options::value<std::string>(&sourceUri)->required(), "Source URI")
    ("dest", boost::program_options::value<std::string>(&destinationUri)->required(), "Destination URI")
  ;
  
  boost::program_options::variables_map vm;
  boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);


  using namespace o2::configuration;
  auto source = ConfigurationFactory::getConfiguration(sourceUri);
  auto values = source->getRecursive("");
  if (destinationUri.substr(0,4) == "json") {
    source->putRecursive(destinationUri.substr(6), values);
    return 0;
  }
  auto destination = ConfigurationFactory::getConfiguration(destinationUri);
  destination->putRecursive("", values);
}
