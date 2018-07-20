/// \file TestBackends.cxx
/// \brief Unit tests for the Configuration.
///
/// \author Adam Wegrzynek, CERN
///

#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Configuration/ConfigurationFactory.h"
#include "Configuration/ConfigurationInterface.h"
#include "Configuration/Visitor.h"
#include "Configuration/Tree.h"

#define BOOST_TEST_MODULE Backends
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

using namespace std::literals::string_literals;
using namespace o2::configuration;

namespace {

BOOST_AUTO_TEST_CASE(TestIllFormedUri)
{
  try {
    ConfigurationFactory::getConfiguration("ini:/bad/uri.ini");
  }
  catch (const std::runtime_error &e) {
    BOOST_CHECK(std::string(e.what()).find("Ill-formed URI") != std::string::npos);
  }
}

BOOST_AUTO_TEST_CASE(IniFileTest)
{
  // Put stuff in temp file
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.ini";
  {
    std::ofstream stream(TEMP_FILE);
    stream <<
      "key=value\n"
        "[section]\n"
        "key_int=123\n"
        "key_float=4.56\n"
        "key_string=hello\n";
  }

  // Get file configuration interface from factory
  auto conf = ConfigurationFactory::getConfiguration("ini:/" + TEMP_FILE);

  std::string key{"/test/key"};
  std::string value{"test_value"};

  // Check with nonexistant keys
  BOOST_CHECK_NO_THROW(BOOST_CHECK(conf->get<int>("this_is/a_bad/key").get_value_or(-1) == -1));

  // File backend does not support putting values
  BOOST_CHECK_THROW(conf->put(key, value), std::runtime_error);

  // Check with default separator
  BOOST_CHECK(conf->get<std::string>("key").get_value_or("") == "value");
  BOOST_CHECK(conf->get<int>("section/key_int").get_value_or(-1) == 123);
  BOOST_CHECK(conf->get<double>("section/key_float").get_value_or(-1.0) == 4.56);
  BOOST_CHECK(conf->get<std::string>("section/key_string").get_value_or("") == "hello");


  // Check with custom separator
  conf->setPathSeparator('.');
  BOOST_CHECK(conf->get<std::string>("key").get_value_or("") == "value");
  BOOST_CHECK(conf->get<int>("section.key_int").get_value_or(-1) == 123);
  BOOST_CHECK(conf->get<double>("section.key_float").get_value_or(-1.0) == 4.56);
  BOOST_CHECK(conf->get<std::string>("section.key_string").get_value_or("") == "hello");
}

BOOST_AUTO_TEST_CASE(JsonFileTest)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";
  {
    std::ofstream stream(TEMP_FILE);
    stream << R"({"menu": {
      "id": "file",
      "popup": {
        "menuitem": {
          "one": {"value": "123", "onclick": "CreateNewDoc"}
        }
      }
    }})";
  }

  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);

  BOOST_CHECK_EQUAL(conf->get<std::string>("menu/id").get_value_or(""), "file");
  BOOST_CHECK_EQUAL(conf->get<std::string>("menu/popup/menuitem/one/onclick").get_value_or(""), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("menu/popup/menuitem/one/value").get_value_or(0), 123);

  BOOST_CHECK_EQUAL(conf->get<int>("menu/popup/menuitem/one/wrong_key").get_value_or(0), 0);
  BOOST_CHECK_EQUAL(conf->get<std::string>("menu/popup/menuitem/one/wrong_key_string").get_value_or("string"), "string");
}

BOOST_AUTO_TEST_CASE(Json2FileRecursiveTest)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";
  {
    std::ofstream stream(TEMP_FILE);
    stream << R"({"menu": {
      "id": "file",
      "popup": {
        "menuitem": {
          "one": {"value": "123", "onclick": "CreateNewDoc"}
        }
      }
    }})";
  }

  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto subTree = conf->getSubTree("menu/popup/menuitem/one");
  BOOST_CHECK_EQUAL(subTree.get<int>("value"), 123);
  BOOST_CHECK_EQUAL(subTree.get<std::string>("onclick"), "CreateNewDoc");
}

} // Anonymous namespace
