/// \file TestJson.cxx
/// \brief JSON backend unit tests.
///
/// \author Adam Wegrzynek, CERN
///

#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Configuration/ConfigurationFactory.h"
#include "Configuration/ConfigurationInterface.h"

#define BOOST_TEST_MODULE JsonBackend
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

using namespace std::literals::string_literals;
using namespace o2::configuration;

namespace {

BOOST_AUTO_TEST_CASE(JsonFileTest)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";
  {
    std::ofstream stream(TEMP_FILE);
    stream << R"({"configuration_library": {
      "id": "file",
      "popup": {
        "menuitem": {
          "one": {"value": "123", "onclick": "CreateNewDoc"}
        }
      }
    }})";
  }

  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);

  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library/id").get_value_or(""), "file");
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library/popup/menuitem/one/onclick").get_value_or(""), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library/popup/menuitem/one/value").get_value_or(0), 123);

  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library/popup/menuitem/one/wrong_key").get_value_or(0), 0);
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library/popup/menuitem/one/wrong_key_string").get_value_or("string"), "string");
}

BOOST_AUTO_TEST_CASE(JsonFileRecursiveTest)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";
  {
    std::ofstream stream(TEMP_FILE);
    stream << R"({"configuration_library": {
      "id": "file",
      "popup": {
        "menuitem": {
          "one": {"value": "123", "onclick": "CreateNewDoc"}
        }
      }
    }})";
  }

  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto subTree = conf->getRecursive("configuration_library");
  BOOST_CHECK_EQUAL(subTree.get<std::string>("id"), "file");
  BOOST_CHECK_EQUAL(subTree.get<int>("popup.menuitem.one.value"), 123);
  BOOST_CHECK_EQUAL(subTree.get<std::string>("popup.menuitem.one.onclick"), "CreateNewDoc");

  auto leaf = conf->getRecursive("configuration_library/popup/menuitem/one");
  BOOST_CHECK_EQUAL(leaf.get<int>("value"), 123);
  BOOST_CHECK_EQUAL(leaf.get<std::string>("onclick"), "CreateNewDoc");

}

BOOST_AUTO_TEST_CASE(JsonFileRecursiveMapTest)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";
  {
    std::ofstream stream(TEMP_FILE);
    stream << R"({"configuration_library": {
      "id": "file",
      "popup": {
        "menuitem": {
          "one": {"value": "123", "onclick": "CreateNewDoc"}
        }
      }
    }})";
  }

  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto map = conf->getRecursiveMap("configuration_library");
  BOOST_CHECK_EQUAL(map.size(), 3);
  BOOST_CHECK_EQUAL(map["id"], "file");
  BOOST_CHECK_EQUAL(map["popup/menuitem/one/onclick"], "CreateNewDoc");
  BOOST_CHECK_EQUAL(map["popup/menuitem/one/value"], "123");

  auto leaf = conf->getRecursiveMap("configuration_library/popup/menuitem/one");
  BOOST_CHECK_EQUAL(leaf.size(), 2);
  BOOST_CHECK_EQUAL(leaf["value"], "123");
  BOOST_CHECK_EQUAL(leaf["onclick"], "CreateNewDoc");
}

} // Anonymous namespace
