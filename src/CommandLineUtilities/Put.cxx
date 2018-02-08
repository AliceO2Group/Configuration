/// \file Put.cxx
/// \brief Command-line utility for putting values to a configuration backend
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "Program.h"
#include "Configuration/ConfigurationFactory.h"

namespace po = boost::program_options;
namespace
{
class Put : public AliceO2::Configuration::Program
{
    virtual Description getDescription() override
    {
      return {"configuration-put", "Puts values to a host",
        "configuration-copy --uri=etcd-v3://host1:2379 --key=foo/bar --value=abc123"};
    }

    virtual void addOptions(boost::program_options::options_description& optionsDescription) override
    {
      optionsDescription.add_options()
          ("uri", po::value<std::string>(&mServerUri)->required(), "Server URI")
          ("key,k", po::value<std::string>(&mKey)->required(), "Key to put to")
          ("value,v", po::value<std::string>(&mValue)->required(), "Value to put");
    }

    virtual void run(const boost::program_options::variables_map& variablesMap) override
    {
      auto configuration = AliceO2::Configuration::ConfigurationFactory::getConfiguration(mServerUri);
      configuration->putString(mKey, mValue);
    }

    std::string mServerUri;
    std::string mKey;
    std::string mValue;
};
} // Anonymous namespace

int main(int argc, char** argv)
{
  return Put().execute(argc, argv);
}
