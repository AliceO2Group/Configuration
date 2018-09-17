///
/// \file Convert.cxx
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "Configuration/ConfigurationFactory.h"
#include <fstream>
#include <boost/program_options.hpp>

using namespace std::literals::string_literals;

int main(int argc, char *argv[]) {
  std::string sourceUri, destinationUri;
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_convert_file.json";

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

  // Workaround for writing JSON
  if (destinationUri.substr(0,4) == "json") {
    {
      std::ofstream stream(TEMP_FILE);
      stream << R"({})";
    }
    auto destination = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
    destination->putRecursive(destinationUri.substr(5), values);
    return 0;
  }

  auto destination = ConfigurationFactory::getConfiguration(destinationUri);
  destination->putRecursive("", values);
}
