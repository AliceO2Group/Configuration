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

  auto conf = ConfigurationFactory::getConfiguration("etcd://aido2qc10:4001");

  std::string key = "/test_dir/test_value";
  int putValue = 123;
  cout << "Assigning value '" << putValue << "' to key '" << key << "'" << endl;
  conf->put<int>(key, 123);

  int getValue = conf->get<int>(key).value();
  cout << "Got key '" << key << "' with value '" << getValue << "'" << endl;

  int getValueWithDefault = conf->get<int>("/this/key/does/not/exist").value_or(-1);
  cout << "Got key '" << key << "' with value '" << getValueWithDefault << "'" << endl;

  return 0;
}
