/// \file EtcdExample.h
/// \brief Example for using the etcd backend
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "Configuration/ConfigurationFactory.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int, char**)
{
  using namespace AliceO2::Configuration;

  std::string myDir = "/test_dir/";
  std::string myKey = "test_key";

  {
    auto conf = ConfigurationFactory::getConfiguration("etcd://aido2qc10:4001");
    auto myPath = myDir + myKey;

    cout << "Assigning value '111' to key '" << myKey << "'" << endl;
    conf->put<int>(myPath, 111);

    int getValue = conf->get<int>(myPath).value();
    cout << "Got key '" << myPath<< "' with value '" << getValue << "'" << endl;

    int getValueWithDefault = conf->get<int>("/this/key/does/not/exist").value_or(-1);
    cout << "Got key '" << myPath << "' with value '" << getValueWithDefault << "'" << endl;
  }

  // Another way to access the same values
  {
    auto conf = ConfigurationFactory::getConfiguration("etcd://aido2qc10:4001/test_dir/");
    conf->put<int>(myKey, 222);
    cout << "Should be 222: " << conf->get<int>(myKey).value() << '\n';
  }

  // Yet another way to access the same values
  {
    auto conf = ConfigurationFactory::getConfiguration("etcd://aido2qc10:4001");
    conf->setPrefix("/test_dir/");
    conf->put<int>(myKey, 333);
    cout << "Should be 333: " << conf->get<int>(myKey).value() << '\n';
  }

  return 0;
}
