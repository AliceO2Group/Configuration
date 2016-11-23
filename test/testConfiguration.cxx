/// \file testConfiguration.cxx
/// \brief example usage of the C++ interface to retrieve configuration parameters from a local file.
///
/// \author Sylvain Chapeland, CERN

#include <fstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <Configuration/ConfigurationFactory.h>

#define BOOST_TEST_MODULE hello test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

using namespace AliceO2::Configuration;

// TODO add assertions

//BOOST_AUTO_TEST_CASE(General_test)
//{
//
//  ConfigFile cfg;
//  try {
//    cfg.load("file:example.cfg");
//
//    int v1;
//    float v2;
//    std::string v3;
//
//
//    // calls with result by reference
//    cfg.getValue("section 1.keyStr1",v3);
//    printf("section 1.keyStr1 = %s\n",v3.c_str());
//
//    cfg.getValue("section 1.keyInt1",v1);
//    printf("section 1.keyInt1 = %d\n",v1);
//
//    cfg.getValue("section 1.keyFloat1",v2);
//    printf("section 1.keyFloat1 = %f\n",v2);
//
//    cfg.getValue("section 2.keyFloat1",v2);
//    printf("section 2.keyFloat1 = %f\n",v2);
//
//
//    // calls with direct result
//    printf("section 1.keyInt1 = %d\n",cfg.getValue<int>("section 1.keyInt1"));
//
//
//    // test non-existing value
//    cfg.getValue("section 3.keyFloat1",v2);
//    printf("section 2.keyFloat1 = %f\n",v2);
//  }
//  catch (std::string err) {
//    printf("%s\n",err.c_str());
//  }
//
//}


//BOOST_AUTO_TEST_CASE(ETCD_test)
//{
//  auto path1 = std::string("/test/path1");
//  auto path2 = std::string("/test/path2");
//  auto path3 = std::string("/test/path3");
//  auto value1 = std::string("value12345");
//  auto value2 = int(12345);
//  auto value3 = double(12.345);
//  auto message = std::string("inserted value not equal to retrieved value");
//
//  // Get ETCD configuration interface from factory
//  auto conf = ConfigurationFactory::getConfiguration("etcd://127.0.0.1:2379");
//
//  // Put and get keys, check equality
//
//  conf->put(path1, value1);
//  BOOST_CHECK_MESSAGE(conf->get<std::string>(path1) == value1, "string: " + message);
//
//  conf->put(path2, value2);
//  BOOST_CHECK_MESSAGE(conf->get<int>(path2) == value2, "int: " + message);
//
//  conf->put(path3, value3);
//  BOOST_CHECK_MESSAGE(conf->get<double>(path3) == value3, "double: " + message);
//}
//

//BOOST_AUTO_TEST_CASE(File_test)
//{
//  try {
//    // Get file configuration interface from factory
//    auto conf = ConfigurationFactory::getConfiguration("file://../../test/example.cfg");
//
//    // Key lookups of different types
//    BOOST_CHECK(conf->get<std::string>("section 1.keyStr1") == "value1");
//    BOOST_CHECK(conf->get<int>("section 1.keyInt1") == 12345);
//    BOOST_CHECK(conf->get<double>("section 1.keyFloat1") == 12.345);
//
//    // Key lookup of nonexistent key
//    try {
//      BOOST_CHECK(conf->get<std::string>("section 123.keyStr123") == "value1");
//      BOOST_FAIL("Did not fail on nonexistent key");
//    } catch (std::runtime_error& e) {
//      // Proper behavior
//    }
//
//    // Attempt to put something in file (not allowed)
//    try {
//      conf->put("section 1.keyStr5", 1);
//      BOOST_FAIL("Did not fail on put to file");
//    } catch (std::runtime_error& e) {
//      // Proper behavior
//    }
//  } catch (std::runtime_error& e) {
//    BOOST_FAIL("Runtime error:" + std::string(e.what()));
//  }
//}

BOOST_AUTO_TEST_CASE(EtcdV3Test)
{
  // Get file configuration interface from factory
  auto conf = ConfigurationFactory::getConfiguration("etcd-v3://aido2qc10:2379");

  std::string key {"/test/key"};
  std::string value {"test_value"};

  conf->put(key, value);
  auto returnedValue = conf->get<std::string>(key);

  BOOST_CHECK(returnedValue.is_initialized());
  BOOST_CHECK(returnedValue.get_value_or("") == value);
}

BOOST_AUTO_TEST_CASE(FileTest)
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
  auto conf = ConfigurationFactory::getConfiguration("file:/"+TEMP_FILE);

  std::string key {"/test/key"};
  std::string value {"test_value"};

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








