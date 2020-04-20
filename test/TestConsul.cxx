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
  auto subTree = conf->getRecursive("configLibTest.tree");
  BOOST_CHECK_EQUAL(subTree.get<int>("one", 1), 1);
  conf->setPrefix("configLibTest");
  BOOST_CHECK_EQUAL(conf->get<std::string>("my_string"), "configuration");
  BOOST_CHECK_EQUAL(conf->get<int>("tree.one"), 1);
}

BOOST_AUTO_TEST_CASE(ConsulArray)
{
  boost::property_tree::ptree tree, children;
  for (int i = 0; i < 3; i++) {
    boost::property_tree::ptree node;
    node.put("one", "1");
    node.put("two", 2);
    children.push_back(std::make_pair("", node));
  }
  tree.put("test_key", "test_value");
  tree.add_child("sample_array", children);

  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  conf->putRecursive("configLibTest", tree);
  auto anArray = conf->getRecursive("configLibTest.sample_array[]");
  for (auto const &it: anArray) {
    BOOST_CHECK_EQUAL(it.second.get<std::string>("one"), "1");
    BOOST_CHECK_EQUAL(it.second.get<int>("two"), 2);
  }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(Dummy)
{
  BOOST_CHECK(true);
}

} // Anonymous namespace
