/// \file TestString.cxx
/// \brief String  backend unit tests.
///
/// \author Adam Wegrzynek, CERN
///

#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Configuration/ConfigurationFactory.h"

#define BOOST_TEST_MODULE StringBackend
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace o2::configuration;

namespace {

BOOST_AUTO_TEST_CASE(StringSimple)
{
  auto conf = ConfigurationFactory::getConfiguration("str://key=value;key2=2;key2.key3=3.3");
  BOOST_CHECK_EQUAL(conf->get<std::string>("key"), "value");
  BOOST_CHECK_EQUAL(conf->get<int>("key2"), 2);
  BOOST_CHECK_EQUAL(conf->get<double>("key2.key3"), 3.3);
}

BOOST_AUTO_TEST_CASE(StringEntireRec)
{
  auto conf = ConfigurationFactory::getConfiguration("str://key=value;key2=2;key2.key3=3.3");
  auto tree = conf->getRecursive();
  BOOST_CHECK_EQUAL(tree.get<std::string>("key"), "value");
  BOOST_CHECK_EQUAL(tree.get<int>("key2"), 2);
  BOOST_CHECK_EQUAL(tree.get<double>("key2.key3"), 3.3);
}


BOOST_AUTO_TEST_CASE(StringSubRec)
{
  auto conf = ConfigurationFactory::getConfiguration("str://key=value;key2=2;key2.key3=3.3");
  auto tree = conf->getRecursive("key2");
  BOOST_CHECK_EQUAL(tree.get<double>("key3"), 3.3);
}

BOOST_AUTO_TEST_CASE(StringSimplePrefix)
{
  auto conf = ConfigurationFactory::getConfiguration("str://key=value;key2=2;key2.key3=3.3");
  conf->setPrefix("key2");
  BOOST_CHECK_EQUAL(conf->get<double>("key3"), 3.3);
}

BOOST_AUTO_TEST_CASE(StringRecMap)
{
  auto conf = ConfigurationFactory::getConfiguration("str://key=value;key2=2;key2.key3=3.3");
  auto map = conf->getRecursiveMap("key2");
  BOOST_CHECK_EQUAL(map["key3"], "3.3");
}

bool exceptionCheck(const std::runtime_error& e)
{
  if (e.what() == std::string("String backend does not support putting values")) {
    return true;
  }
  return false;
}

BOOST_AUTO_TEST_CASE(StringPutExc)
{
  auto conf = ConfigurationFactory::getConfiguration("str://key=value;key2=2;key2.key3=3.3");
  BOOST_CHECK_EXCEPTION(conf->put<int>("key2.key4", 4), std::runtime_error, exceptionCheck);
}

} // Anonymous namespace
