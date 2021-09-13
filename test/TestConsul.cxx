/// \file TestConsul.cxx
/// \brief Unit tests for the Configuration.
///
/// \author Adam Wegrzynek, CERN
///

#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Configuration/ConfigurationFactory.h"
#include "Configuration/ConfigurationInterface.h"

#define BOOST_TEST_MODULE ConsulBackend
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace std::literals::string_literals;
using namespace o2::configuration;

namespace {

const std::string CONSUL_ENDPOINT = "127.0.0.1:8500";


BOOST_AUTO_TEST_SUITE(optionalTest, * boost::unit_test::disabled())


BOOST_AUTO_TEST_CASE(simpleReadWrite)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->put<int>("configLibTest.my_id", 123);
  BOOST_CHECK_EQUAL(conf->get<int>("configLibTest.my_id"), 123);

  conf->put<std::string>("configLibTest.my_string", "configuration");
  BOOST_CHECK_EQUAL(conf->get<std::string>("configLibTest.my_string"), "configuration");

  conf->put<double>("configLibTest.pi", 3.14);
  BOOST_CHECK_EQUAL(conf->get<double>("configLibTest.pi"), 3.14);

  BOOST_CHECK_THROW(conf->get<int>("configuration_library.popup.menuitem.one.wrong_key"), std::runtime_error);
  BOOST_CHECK_THROW(conf->get<std::string>("configuration_library.popup.menuitem.one.wrong_key_string"), std::runtime_error);

  std::string iniContent = "key=value\n"
    "[section]\n"
    "key_int=123\n"
    "key_float=4.56\n"
    "key_string=hello\n"; 
  conf->put<std::string>("configLibTest.file.ini", iniContent);

  std::string jsonContent = R"({"configuration_library": {
    "id": "file",
    "complex_array": [
      {"host": "127.0.0.1", "port": 123},
      {"host": "192.168.1.1", "port": 123},
      {"host": "255.0.0.0", "port": 9012}
    ]
  }})";
  conf->put<std::string>("configLibTest.file.json", jsonContent);
}

BOOST_AUTO_TEST_CASE(ConsulMap)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->put<int>("configLibTest.tree.one", 1);
  conf->put<int>("configLibTest.tree.two", 2);
  conf->put<int>("configLibTest.tree.three", 3);

  auto leaf = conf->getRecursiveMap("configLibTest.tree");
  BOOST_CHECK_EQUAL(leaf.size(), 3);
  BOOST_CHECK_EQUAL(leaf["one"], "1");
  BOOST_CHECK_EQUAL(leaf["two"], "2");
  BOOST_CHECK_EQUAL(leaf["three"], "3");
}

BOOST_AUTO_TEST_CASE(ConsulPtree)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  auto subTree = conf->getRecursive("configLibTest");
  BOOST_CHECK_EQUAL(subTree.get<std::string>("my_string"), "configuration");
  BOOST_CHECK_EQUAL(subTree.get<int>("tree.one"), 1);
}

BOOST_AUTO_TEST_CASE(ConsulPrefix)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->setPrefix("configLibTest");
  auto subTree = conf->getRecursive("tree");
  BOOST_CHECK_EQUAL(subTree.get<int>("one"), 1);
  BOOST_CHECK_EQUAL(conf->get<std::string>("my_string"), "configuration");
  BOOST_CHECK_EQUAL(conf->get<int>("tree.one"), 1);
}

BOOST_AUTO_TEST_CASE(ConsulPrefix2)
{
  auto confPath = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT + "/configLibTest");
  auto subTreePath = confPath->getRecursive("tree");
  BOOST_CHECK_EQUAL(subTreePath.get<int>("one"), 1);
  confPath->setPrefix("tree");
  BOOST_CHECK_EQUAL(confPath->get<int>("one"), 1);
}

BOOST_AUTO_TEST_CASE(ConsulIni)
{
  auto conf = ConfigurationFactory::getConfiguration("consul-ini://" + CONSUL_ENDPOINT + "/configLibTest.file.ini");
  BOOST_CHECK_EQUAL(conf->get<std::string>("key"), "value");
  BOOST_CHECK_EQUAL(conf->get<int>("section.key_int"), 123);
  BOOST_CHECK_EQUAL(conf->get<double>("section.key_float"), 4.56);
  BOOST_CHECK_EQUAL(conf->get<std::string>("section.key_string"), "hello");
}

BOOST_AUTO_TEST_CASE(ConsulIni2)
{
  auto conf = ConfigurationFactory::getConfiguration("consul-ini://" + CONSUL_ENDPOINT + "/configLibTest.file.ini");
  auto tree = conf->getRecursive();
  BOOST_CHECK_EQUAL(tree.get<std::string>("key"), "value");
  BOOST_CHECK_EQUAL(tree.get<double>("section.key_float"), 4.56);
  BOOST_CHECK_EQUAL(tree.get<std::string>("section.key_string"), "hello");
}

BOOST_AUTO_TEST_CASE(ConsulJson)
{
  auto conf = ConfigurationFactory::getConfiguration("consul-json://" + CONSUL_ENDPOINT + "/configLibTest.file.json");
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.id"), "file");

  auto subTree = conf->getRecursive("");
  BOOST_CHECK_EQUAL(subTree.get<std::string>("configuration_library.id"), "file");

  auto anArray = conf->getRecursive("configuration_library.complex_array");
  int ports = 0;
  std::string hosts = ""; 
  for (auto const &it: anArray) {
    hosts += it.second.get<std::string>("host");
    ports += it.second.get<int>("port");
  }
  BOOST_CHECK_EQUAL(ports, 9258);
  BOOST_CHECK_EQUAL(hosts, "127.0.0.1192.168.1.1255.0.0.0");
 
}
  
BOOST_AUTO_TEST_CASE(ConsulJsonInvalid)
{
  BOOST_CHECK_THROW(ConfigurationFactory::getConfiguration("consul-json://" + CONSUL_ENDPOINT + "/invalid.json"), std::runtime_error);
}
  
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(Dummy)
{
  BOOST_CHECK(true);
}

} // Anonymous namespace
