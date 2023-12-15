/// \file TestApricot.cxx
/// \brief Unit tests for the Configuration.
///
/// \author Adam Wegrzynek, CERN
///

#include "Configuration/ConfigurationFactory.h"
#include "Configuration/ConfigurationInterface.h"

#define BOOST_TEST_MODULE ApricotBackend
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace o2::configuration;

namespace {

const std::string APRICOT_ENDPOINT = "127.0.0.1:32188";


BOOST_AUTO_TEST_SUITE(optionalTest, * boost::unit_test::disabled())


BOOST_AUTO_TEST_CASE(simpleCheck)
{
  auto conf = ConfigurationFactory::getConfiguration("apricot://" + APRICOT_ENDPOINT);
  auto tree = conf->getRecursive("components.qc.ANY.any.tpc-full-qcmn");
  BOOST_CHECK_EQUAL(tree.get<std::string>("qc.config.database.implementation"), "CCDB");
  BOOST_CHECK_EQUAL(tree.get<std::string>("qc.tasks.RawDigits.moduleName"), "QcTPC");
  
}


BOOST_AUTO_TEST_CASE(simpleCheckQuery)
{
  auto conf = ConfigurationFactory::getConfiguration("apricot://" + APRICOT_ENDPOINT + "/components/qc/ANY/any/tpc-full-qcmn?process=true&run_type=PHYSICS");
  auto tree = conf->getRecursive("");
  BOOST_CHECK_EQUAL(tree.get<std::string>("qc.config.database.implementation"), "CCDB");
  BOOST_CHECK_EQUAL(tree.get<std::string>("qc.tasks.RawDigits.moduleName"), "QcTPC");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(Dummy)
{
  BOOST_CHECK(true);
}

} // Anonymous namespace
