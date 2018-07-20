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
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library/sample1").get_value_or(""), "123");
  BOOST_CHECK_EQUAL(conf->get<std::string>("configuration_library/test_dir/dir_value").get_value_or(""), "321");
  
  BOOST_CHECK_EQUAL(conf->get<int>("configuration_library/test_dir/dir_value").get_value_or(0), 321);
}

BOOST_AUTO_TEST_CASE(ConsulMap)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  auto map = conf->getRecursiveMap("configuration_library/test_dir");
  BOOST_CHECK_EQUAL(map.size(), 2);
  BOOST_CHECK_EQUAL(map["/dir_value"], "321");
}

BOOST_AUTO_TEST_CASE(ConsulPtree)
{
  auto conf = ConfigurationFactory::getConfiguration("consul://" + CONSUL_ENDPOINT);
  auto subTree = conf->getSubTree("configuration_library/test_dir");
  BOOST_CHECK_EQUAL(subTree.get<int>("/dir_value"), 321);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(Dummy)
{
  BOOST_CHECK(true);
}

} // Anonymous namespace
