/// \file Get.cxx
/// \brief Command-line utility for getting values from a configuration backend
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "Common/Program.h"
#include "Configuration/ConfigurationFactory.h"

namespace po = boost::program_options;
namespace
{
class Get : public AliceO2::Common::Program
{
    virtual Description getDescription() override
    {
      return {"configuration-get", "Gets values from a host",
        "configuration-get --uri=etcd-v3://host1:2379 --key=foo/bar"};
    }

    virtual void addOptions(boost::program_options::options_description& optionsDescription) override
    {
      optionsDescription.add_options()
          ("uri", po::value<std::string>(&mServerUri)->required(), "Server URI")
          ("key,k", po::value<std::string>(&mKey)->required(), "Key to get value with")
          ("recursive,r", po::bool_switch(&mRecursive), "Recursive get");
    }

    virtual void run(const boost::program_options::variables_map& variablesMap) override
    {
      auto configuration = AliceO2::Configuration::ConfigurationFactory::getConfiguration(mServerUri);
      if (mRecursive) {
        AliceO2::Configuration::Tree::printTree(configuration->getRecursive(mKey), std::cout);
      } else {
        std::cout << configuration->getString(mKey).value_or("Key did not exist") << '\n';
      }
    }

    std::string mServerUri;
    std::string mKey;
    bool mRecursive;
};
} // Anonymous namespace

int main(int argc, char** argv)
{
  return Get().execute(argc, argv);
}
