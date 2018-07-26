/// \file TestExamples.cxx
/// \brief Contains examples of the Configuration API. They are referenced from other parts of the documentation.
///
/// Keeping these examples as a unit tests means the user has some assurance they are actually working.
/// Additionally: unlike examples in comments, these will start complaining if we don't maintain them.
///
/// \author Pascal Boeschoten, CERN

#include <iostream>
#include "Configuration/ConfigurationFactory.h"

#define BOOST_TEST_MODULE hello test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

using namespace std::literals::string_literals;
using namespace o2::configuration;
using std::cout;
using std::endl;

namespace
{

BOOST_AUTO_TEST_CASE(Example)
{
  std::string myDir = "/test_dir/";
  std::string myKey = "test_key";
  std::string uri = "etcd-v3://localhost:2379";

  try {
    {
      // URI for connecting to local etcd server
      auto uri = "etcd-v3://localhost:2379";

      // Get a Configuration interface to etcd
      std::unique_ptr<ConfigurationInterface> conf = ConfigurationFactory::getConfiguration(uri);
      auto myKey = "/test_dir/test_key";
      int myValue = 111;

      // Putting & getting
      conf->put<int>(myKey, myValue);
      conf->get<int>(myKey);
      conf->get<int>("/this/key/does/not/exist");


      // Assuming our backend has these key-value pairs:
      conf->put<int>("/dir/myBool", 0);
      conf->put<int>("/dir/subdir/myInt", 123);
      conf->put<std::string>("/dir/subdir/subsubdir/myString", "abc");
    }

    {
      auto conf = ConfigurationFactory::getConfiguration(uri);
      auto myPath = myDir + myKey;

      cout << "Assigning value '111' to key '" << myKey << "'" << endl;
      conf->put<int>(myPath, 111);

      int getValue = conf->get<int>(myPath);
      cout << "Got key '" << myPath << "' with value '" << getValue << "'" << endl;

      int getValueWithDefault = conf->get<int>("/this/key/does/not/exist");
      cout << "Got key '" << myPath << "' with value '" << getValueWithDefault << "'" << endl;
    }

    // Another way to access the same values
    {
      auto conf = ConfigurationFactory::getConfiguration(uri + "/test_dir/");
      conf->put<int>(myKey, 222);
      cout << "Should be 222: " << conf->get<int>(myKey) << '\n';
    }

    // Yet another way to access the same values
    {
      auto conf = ConfigurationFactory::getConfiguration(uri);
      conf->setPrefix("/test_dir/");
      conf->put<int>(myKey, 333);
      cout << "Should be 333: " << conf->get<int>(myKey) << '\n';
    }
  }
  catch (const std::runtime_error& e) {
    BOOST_WARN_MESSAGE(false,
        std::string("Exception thrown, you may be missing the required infrastructure: ") + e.what());
  }
}

} // Anonymous namespace
