/// \file testConfiguration.cxx
/// \brief example usage of the C++ interface to retrieve configuration parameters from a local file.
///
/// \author Sylvain Chapeland, CERN

#include <Configuration/Configuration.h>
#include <stdio.h>
#include <string>

#define BOOST_TEST_MODULE hello test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

// TODO add assertions

BOOST_AUTO_TEST_CASE(General_test)
{

  ConfigFile cfg;
  try {
    cfg.load("file:example.cfg"); 
  
    int v1;
    float v2;
    std::string v3;


    // calls with result by reference
    cfg.getValue("section 1.keyStr1",v3);
    printf("section 1.keyStr1 = %s\n",v3.c_str());
  
    cfg.getValue("section 1.keyInt1",v1);
    printf("section 1.keyInt1 = %d\n",v1);

    cfg.getValue("section 1.keyFloat1",v2);
    printf("section 1.keyFloat1 = %f\n",v2);

    cfg.getValue("section 2.keyFloat1",v2);
    printf("section 2.keyFloat1 = %f\n",v2);


    // calls with direct result
    printf("section 1.keyInt1 = %d\n",cfg.getValue<int>("section 1.keyInt1"));


    // test non-existing value
    cfg.getValue("section 3.keyFloat1",v2);
    printf("section 2.keyFloat1 = %f\n",v2);
  }
  catch (std::string err) {
    printf("%s\n",err.c_str());
  }

}
