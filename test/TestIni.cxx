/// \file TestIni.cxx
/// \brief INI file backend unit tests.
///
/// \author Adam Wegrzynek, CERN
///

#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Configuration/ConfigurationFactory.h"
#include "Configuration/ConfigurationInterface.h"
#include "../src/Backends/Ini/IniBackend.h"

#define BOOST_TEST_MODULE IniBackend
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

const std::string TEMP_FILE = "/tmp/alice_o2_configuration_test_file.ini";
BOOST_AUTO_TEST_CASE(IniFileTest)
{
  // Put stuff in temp file
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

  std::string key{"test.key"};
  std::string value{"test_value"};

  // Check with nonexistant keys
  BOOST_CHECK_NO_THROW(BOOST_CHECK(conf->get<int>("this_is.a_bad.key", -1) == -1));

  // File backend does not support putting values
  BOOST_CHECK_THROW(conf->put(key, value), std::runtime_error);

  // Throw when default value not provided
  BOOST_CHECK_THROW(conf->get<int>("this_is.a_bad.key"), std::runtime_error);

  // Check with default separator
  BOOST_CHECK(conf->get<std::string>("key") == "value");
  BOOST_CHECK(conf->get<int>("section.key_int") == 123);
  BOOST_CHECK(conf->get<double>("section.key_float") == 4.56);
  BOOST_CHECK(conf->get<std::string>("section.key_string") == "hello");
}

BOOST_AUTO_TEST_CASE(IniFileTestPtreeGetTotal)
{
  auto conf = ConfigurationFactory::getConfiguration("ini:/" + TEMP_FILE);
  auto tree = conf->getRecursive();
  BOOST_CHECK_EQUAL(tree.get<std::string>("key"), "value");
  BOOST_CHECK_EQUAL(tree.get<double>("section.key_float"), 4.56);
  BOOST_CHECK_EQUAL(tree.get<std::string>("section.key_string"), "hello");
}

BOOST_AUTO_TEST_CASE(IniFileTestPtreeGetSub)
{
  auto conf = ConfigurationFactory::getConfiguration("ini:/" + TEMP_FILE);
  auto tree = conf->getRecursive("section");
  BOOST_CHECK_EQUAL(tree.get<double>("key_float"), 4.56);
  BOOST_CHECK_EQUAL(tree.get<std::string>("key_string"), "hello");
}

BOOST_AUTO_TEST_CASE(IniFileTestPrefix)
{
  auto conf = ConfigurationFactory::getConfiguration("ini:/" + TEMP_FILE);
  conf->setPrefix("section");
  BOOST_CHECK_EQUAL(conf->get<int>("key_int"), 123);
  BOOST_CHECK_EQUAL(conf->get<double>("key_float"), 4.56);
  BOOST_CHECK_EQUAL(conf->get<std::string>("key_string"), "hello");
}

BOOST_AUTO_TEST_CASE(IniFileUseStream)
{
  std::string iniContent = "key=value\n"
        "[section]\n"
        "key_int=123\n"
        "key_float=4.56\n"
        "key_string=hello\n";
  auto conf = std::make_unique<backends::IniBackend>(iniContent, true);
  BOOST_CHECK(conf->get<std::string>("key") == "value");
  BOOST_CHECK(conf->get<int>("section.key_int") == 123);
}

} // Anonymous namespace
