/// \file TestTree.cxx
/// \brief Unit tests for the Tree classes and functions
///
/// \author Pascal Boeschoten, CERN

#include <iostream>
#include "Configuration/Visitor.h"
#include "Configuration/Tree.h"

#define BOOST_TEST_MODULE hello test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

namespace
{

using namespace std::literals::string_literals;
using namespace o2::configuration;

BOOST_AUTO_TEST_CASE(TreeTest)
{
  using namespace tree;

  Node tree = Branch
  {
    {"equipment_1", Branch
      {
        {"enabled", true},
        {"type", "rorc"s},
        {"serial", 33333},
        {"channel", 0},
        {"stuff", Branch
          {
            {"abc", 123},
            {"xyz", 456}
          }
        }
      }
    },
    {"equipment_2", Branch
      {
        {"enabled", true},
        {"type", "dummy"s},
        {"serial", -1},
        {"channel", 0}
      }
    }
  };

  // Get a branch and some values from it
  Branch eq1 = getBranch(tree, "equipment_1");
  BOOST_CHECK(getRequired<bool>(eq1, "enabled") == true);
  BOOST_CHECK(getRequired<std::string>(eq1, "type") == "rorc"s);


  // Get optional value (wrapped in boost::optional)
  BOOST_CHECK(get<int>(eq1, "nothing_here").get_value_or(-1) == -1);

  // Extract subtree and get an int value from a leaf
  BOOST_CHECK(getRequired<int>(getSubtree(tree, "equipment_1/stuff/abc")) == 123);
}

/// Tests the conversion of key value pairs to a tree
BOOST_AUTO_TEST_CASE(KeyValuePairConversionTest)
{
  using namespace tree;

  std::vector<std::pair<std::string, Leaf>> referencePairs {
      {"/dir/bool", false},
      {"/dir/double", 45.6},
      {"/dir/subdir/int", 123},
      {"/dir/subdir/subsubdir/string", "string"s}};

  Node referenceTree = Branch {
      {"dir", Branch {
        {"bool", false},
        {"double", 45.6},
        {"subdir", Branch {
          {"int", 123},
          {"subsubdir", Branch {
            {"string", "string"s}}}}}}}};

  Node convertedTree = keyValuesToTree(referencePairs);
  BOOST_CHECK(referenceTree == convertedTree);
  auto convertedPairs = treeToKeyValues(convertedTree);
  BOOST_CHECK(referencePairs == convertedPairs);
}

} // Anonymous namespace
