/// \file Examples.cxx
/// \brief Contains examples of the Configuration API. They are referenced from other parts of the documentation.
///
/// Keeping these examples as a unit tests means the user has some assurance they are actually working.
/// Additionally: unlike examples in comments, these will start complaining if we don't maintain them.
///
/// \author Pascal Boeschoten, CERN

#include <iostream>
#include "Configuration/ConfigurationFactory.h"
#include "Configuration/Visitor.h"
#include "Configuration/Tree.h"

#define BOOST_TEST_MODULE hello test
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <assert.h>

using namespace std::literals::string_literals;
using namespace AliceO2::Configuration;
using std::cout;
using std::endl;

namespace
{

BOOST_AUTO_TEST_CASE(TreeExample)
{
  using namespace Tree;

  //! [Tree declaration]
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
  //! [Tree declaration]

  //! [Tree getters]
  // Get a branch and some values from it
  Branch eq1 = getBranch(tree, "equipment_1");
  assert(getRequired<bool>(eq1, "enabled") == true);
  assert(getRequired<std::string>(eq1, "type") == "rorc"s);

  // Get optional value (wrapped in boost::optional)
  assert(get<int>(eq1, "nothing_here").get_value_or(-1) == -1);
  //! [Tree getters]

  //! [Tree traversal]
  // Extract subtree and get an int value from a leaf
  assert(getRequired<int>(getSubtree(tree, "equipment_1/stuff/abc")) == 123);
  Node a = getSubtree(tree, "equipment_1/stuff/abc");
  // Manually traverse
  Node b = getLeaf(getBranch(getBranch(tree, "equipment_1"), "stuff"), "abc");
  assert(a == b);
  //! [Tree traversal
}

BOOST_AUTO_TEST_CASE(ConversionExample)
{
  using namespace Tree;

  //! [Key-value pair conversion]
  std::vector<std::pair<std::string, Leaf>> pairs {
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

  Node convertedTree = keyValuesToTree(pairs);
  assert(referenceTree == convertedTree);
  //! [Key-value pair conversion]
}

BOOST_AUTO_TEST_CASE(SubtreeExample)
{
  using namespace Tree;

  //! [Get subtree]
  auto tree = Branch {
      {"dir", Branch {
        {"subdir", Branch {
          {"value", 123}}}}}};
  getRequired<int>(getSubtree(tree, "/dir/subdir/value"));
  //! [Get subtree]
}

BOOST_AUTO_TEST_CASE(VisitorExample)
{
  //! [Visitor]
  auto variant = boost::variant<int, std::string>(42);
  auto visitor = Visitor::make<std::string>(
      [](int i)        { return "It's an int!"; },
      [](std::string s){ return "It's a string!"; });
  auto whatIsIt = boost::apply_visitor(visitor, variant);
  assert(whatIsIt == "It's an int!");
  //! [Visitor]
}

BOOST_AUTO_TEST_CASE(ApplyVisitorExample)
{
  //! [Apply visitor]
  auto variant = boost::variant<int, std::string>(42);
  auto whatIsIt = Visitor::apply<std::string>(variant,
      [](int i)        { return "It's an int!"; },
      [](std::string s){ return "It's a string!"; });
  assert(whatIsIt == "It's an int!");
  //! [Apply visitor]
}

} // Anonymous namespace
