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

const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";

BOOST_AUTO_TEST_CASE(JsonFileTest)
{
  {
    std::ofstream stream(TEMP_FILE);
    stream << R"({"configuration_library": {
      "id": "file",
      "array[]": [
        "zero",
        "un",
        "deux"
      ],
      "complex_array[]": [
        {"host": "127.0.0.1", "port": 123},
        {"host": "192.168.1.1", "port": 123},
        {"host": "255.0.0.0", "port": 9012}
      ],
      "popup": {
        "menuitem": {
          "one": {"value": "123", "onclick": "CreateNewDoc"}
        }
      }
    }})";
  }

  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);

  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.id"), "file");
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.popup.menuitem.one.onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library.popup.menuitem.one.value"), 123);

  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library.popup.menuitem.one.wrong_key", 0), 0);
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.popup.menuitem.one.wrong_key_string", "string"), "string");

  BOOST_CHECK_THROW(conf->get<int>("configuration_library.popup.menuitem.one.wrong_key"), std::runtime_error);
  BOOST_CHECK_THROW(conf->get<std::string>("configuration_library.popup.menuitem.one.wrong_key_string"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(JsonFileRecursiveTest)
{
  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto subTree = conf->getRecursive("configuration_library");
  BOOST_CHECK_EQUAL(subTree.get<std::string>("id"), "file");
  BOOST_CHECK_EQUAL(subTree.get<int>("popup.menuitem.one.value"), 123);
  BOOST_CHECK_EQUAL(subTree.get<std::string>("popup.menuitem.one.onclick"), "CreateNewDoc");

  auto leaf = conf->getRecursive("configuration_library.popup.menuitem.one");
  BOOST_CHECK_EQUAL(leaf.get<int>("value"), 123);
  BOOST_CHECK_EQUAL(leaf.get<std::string>("onclick"), "CreateNewDoc");

}

BOOST_AUTO_TEST_CASE(JsonFileRecursiveMapTest)
{
  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto map = conf->getRecursiveMap("configuration_library");
  BOOST_CHECK_EQUAL(map["id"], "file");
  BOOST_CHECK_EQUAL(map["popup.menuitem.one.onclick"], "CreateNewDoc");
  BOOST_CHECK_EQUAL(map["popup.menuitem.one.value"], "123");

  auto leaf = conf->getRecursiveMap("configuration_library.popup.menuitem.one");
  BOOST_CHECK_EQUAL(leaf["value"], "123");
  BOOST_CHECK_EQUAL(leaf["onclick"], "CreateNewDoc");
}

BOOST_AUTO_TEST_CASE(JsonFilePrefix)
{
  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  conf->setPrefix("configuration_library");
  BOOST_CHECK_EQUAL(conf->get<std::string>("id"), "file");
  BOOST_CHECK_EQUAL(conf->get<std::string>("popup.menuitem.one.onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("popup.menuitem.one.value"), 123);

  conf->setPrefix("configuration_library.popup.menuitem.one");
  BOOST_CHECK_EQUAL(conf->get<std::string>("onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("value"), 123);
}

BOOST_AUTO_TEST_CASE(JsonFileArray)
{
  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto anArray = conf->getRecursive("configuration_library.array[]");
  std::string merged = "";
  std::string keys = "";
  for (auto const &it: anArray) {
    keys += it.first;
    merged += it.second.data();
  }
  BOOST_CHECK_EQUAL(keys, "");
  BOOST_CHECK_EQUAL(merged, "zeroundeux");
}

BOOST_AUTO_TEST_CASE(JsonFileNestedArray)
{
  auto conf = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto anArray = conf->getRecursive("configuration_library.complex_array[]");
  int ports = 0;
  std::string hosts = "";
  for (auto const &it: anArray) {
    hosts += it.second.get<std::string>("host");
    ports += it.second.get<int>("port");
  }
  BOOST_CHECK_EQUAL(ports, 9258);
  BOOST_CHECK_EQUAL(hosts, "127.0.0.1192.168.1.1255.0.0.0");
}

} // Anonymous namespace
