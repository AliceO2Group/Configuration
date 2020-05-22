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

BOOST_AUTO_TEST_CASE(ConsulJson)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.json";
  auto confJson = ConfigurationFactory::getConfiguration("json:/" + TEMP_FILE);
  auto tree = confJson->getRecursive("configuration_library.popup");
  BOOST_CHECK_EQUAL(tree.get<int>("menuitem.one.value"), 123);

  std::ifstream jsonFile(TEMP_FILE);
  std::stringstream buffer;
  buffer << jsonFile.rdbuf();

  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->put<std::string>("configLibTest.json", buffer.str());
  auto json = conf->getRecursive("configLibTest.json");

  BOOST_CHECK_EQUAL(json.get<std::string>("configuration_library.id"), "file");
  BOOST_CHECK_EQUAL(json.get<std::string>("configuration_library.popup.menuitem.one.onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(json.get<int>("configuration_library.popup.menuitem.one.value"), 123);
}

BOOST_AUTO_TEST_CASE(ConsulIni)
{
  const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.ini";
  auto confIni = ConfigurationFactory::getConfiguration("ini:/" + TEMP_FILE);
  auto tree = confIni->getRecursive("section");
  BOOST_CHECK_EQUAL(tree.get<double>("key_float"), 4.56);

  std::ifstream iniFile(TEMP_FILE);
  std::stringstream buffer;
  buffer << iniFile.rdbuf();

  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->put<std::string>("configLibTest.ini", buffer.str());
  auto ini = conf->getRecursive("configLibTest.ini");
  BOOST_CHECK_EQUAL(ini.get<int>("section.key_int"), 123);
  BOOST_CHECK_EQUAL(ini.get<std::string>("section.key_string"), "hello");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(Dummy)
{
  BOOST_CHECK(true);
}

} // Anonymous namespace
