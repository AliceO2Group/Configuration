/*
 * Example.cxx
 *
 *  Created on: Jul 1, 2016
 *      Author: pboescho
 */

#include "Configuration/ConfigurationFactory.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
  auto conf = ConfigurationFactory::getConfiguration("etcd://aido2qc10:4001");

  std::string key = "/test_dir/test_value";
  int putValue = 123;
  cout << "Assigning value '" << putValue << "' to key '" << key << "'" << endl;
  conf->put<int>(key, 123);

  int getValue = conf->get<int>(key);
  cout << "Got key '" << key << "' with value '" << getValue << "'" << endl;

  return 0;
}


