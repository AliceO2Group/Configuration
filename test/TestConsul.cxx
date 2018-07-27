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
BOOST_AUTO_TEST_CASE(ConsulBackend)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.id"), "file");

  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.popup.menuitem.one.onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library.popup.menuitem.one.value"), 123);

  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library.popup.menuitem.one.wrong_key", 0), 0); 
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library.popup.menuitem.one.wrong_key_string", "string"), "string");

  BOOST_CHECK_THROW(conf->get<int>("configuration_library.popup.menuitem.one.wrong_key"), std::runtime_error);
  BOOST_CHECK_THROW(conf->get<std::string>("configuration_library.popup.menuitem.one.wrong_key_string"), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(ConsulMap)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  auto map = conf->getRecursiveMap("configuration_library");
  BOOST_CHECK_EQUAL(map.size(), 3);
  BOOST_CHECK_EQUAL(map["id"], "file");
  BOOST_CHECK_EQUAL(map["popup.menuitem.one.onclick"], "CreateNewDoc");
  BOOST_CHECK_EQUAL(map["popup.menuitem.one.value"], "123");

  auto leaf = conf->getRecursiveMap("configuration_library.popup.menuitem.one");
  BOOST_CHECK_EQUAL(leaf.size(), 2);
  BOOST_CHECK_EQUAL(leaf["value"], "123");
  BOOST_CHECK_EQUAL(leaf["onclick"], "CreateNewDoc");
}

BOOST_AUTO_TEST_CASE(ConsulPtree)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  auto subTree = conf->getRecursive("configuration_library");
  BOOST_CHECK_EQUAL(subTree.get<std::string>("id"), "file");
  BOOST_CHECK_EQUAL(subTree.get<int>("popup.menuitem.one.value"), 123);
  BOOST_CHECK_EQUAL(subTree.get<std::string>("popup.menuitem.one.onclick"), "CreateNewDoc");

  auto leaf = conf->getRecursive("configuration_library.popup.menuitem.one");
  BOOST_CHECK_EQUAL(leaf.get<int>("value"), 123);
  BOOST_CHECK_EQUAL(leaf.get<std::string>("onclick"), "CreateNewDoc");
}

BOOST_AUTO_TEST_CASE(ConsulPrefix)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->setPrefix("configuration_library");
  BOOST_CHECK_EQUAL(conf->get<std::string>("id"), "file");
  BOOST_CHECK_EQUAL(conf->get<std::string>("popup.menuitem.one.onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("popup.menuitem.one.value"), 123);

  conf->setPrefix("configuration_library.popup.menuitem.one");
  BOOST_CHECK_EQUAL(conf->get<std::string>("onclick"), "CreateNewDoc");
  BOOST_CHECK_EQUAL(conf->get<int>("value"), 123);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(Dummy)
{
  BOOST_CHECK(true);
}

} // Anonymous namespace
