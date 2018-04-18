/// \file TestExamples.cxx
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
using namespace o2::configuration;
using std::cout;
using std::endl;

namespace
{

BOOST_AUTO_TEST_CASE(TreeExample)
{
  using namespace tree;

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
  using namespace tree;

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
  using namespace tree;

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
  auto visitor = visitor::make<std::string>(
      [](int)        { return "It's an int!"; },
      [](std::string){ return "It's a string!"; });
  auto whatIsIt = boost::apply_visitor(visitor, variant);
  assert(whatIsIt == "It's an int!");
  //! [Visitor]
}

BOOST_AUTO_TEST_CASE(ApplyVisitorExample)
{
  //! [Apply visitor]
  auto variant = boost::variant<int, std::string>(42);
  auto whatIsIt = visitor::apply<std::string>(variant,
      [](int)        { return "It's an int!"; },
      [](std::string){ return "It's a string!"; });
  assert(whatIsIt == "It's an int!");
  //! [Apply visitor]
}

BOOST_AUTO_TEST_CASE(Example)
{
  std::string myDir = "/test_dir/";
  std::string myKey = "test_key";
  std::string uri = "etcd-v3://localhost:2379";

  try {
    {
      // URI for connecting to local etcd server
      auto uri = "etcd-v3://localhost:2379";

      // Get a Configuration interface to etcd
      std::unique_ptr<ConfigurationInterface> conf = ConfigurationFactory::getConfiguration(uri);
      auto myKey = "/test_dir/test_key";
      int myValue = 111;

      // Putting & getting
      conf->put<int>(myKey, myValue);
      // Getters return a boost::optional that will be empty if the value is not present
      conf->get<int>(myKey).value();
      conf->get<int>("/this/key/does/not/exist").value_or(-1);


      // Assuming our backend has these key-value pairs:
      conf->put<int>("/dir/myBool", 0);
      conf->put<int>("/dir/subdir/myInt", 123);
      conf->put<std::string>("/dir/subdir/subsubdir/myString", "abc");

      // We can do a recursive get, which gives us a tree data structure
      // A Node can be either:
      //   * a Branch, which represents a directory or path segment. It's implemented as a map, where the key is
      //     the path segment and the value is a Node
      //   * a Leaf, which represents a value. It's a boost::variant that can contain a string, int, double or bool
      tree::Node tree = conf->getRecursive("/");

      // Then we can get a Branch and some values from it
      tree::Node dir = tree::getBranch(tree, "dir");
      assert(tree::getRequired<bool>(dir, "myBool") == false);

      // We can also traverse multiple steps
      tree::Node subdir = tree::getSubtree(tree, "/dir/subdir");
      assert(tree::getRequired<int>(subdir, "myInt") == 123);

      // Traversal works down to leaf nodes
      tree::Node myString = tree::getSubtree(tree, "/dir/subdir/subsubdir/myString");
      assert(tree::getRequired<std::string>(myString) == "abc");
    }

    {
      auto conf = ConfigurationFactory::getConfiguration(uri);
      auto myPath = myDir + myKey;

      cout << "Assigning value '111' to key '" << myKey << "'" << endl;
      conf->put<int>(myPath, 111);

      int getValue = conf->get<int>(myPath).value();
      cout << "Got key '" << myPath << "' with value '" << getValue << "'" << endl;

      int getValueWithDefault = conf->get<int>("/this/key/does/not/exist").value_or(-1);
      cout << "Got key '" << myPath << "' with value '" << getValueWithDefault << "'" << endl;
    }

    // Another way to access the same values
    {
      auto conf = ConfigurationFactory::getConfiguration(uri + "/test_dir/");
      conf->put<int>(myKey, 222);
      cout << "Should be 222: " << conf->get<int>(myKey).value() << '\n';
    }

    // Yet another way to access the same values
    {
      auto conf = ConfigurationFactory::getConfiguration(uri);
      conf->setPrefix("/test_dir/");
      conf->put<int>(myKey, 333);
      cout << "Should be 333: " << conf->get<int>(myKey).value() << '\n';
    }
  }
  catch (const std::runtime_error& e) {
    BOOST_WARN_MESSAGE(false,
        std::string("Exception thrown, you may be missing the required infrastructure: ") + e.what());
  }
}

} // Anonymous namespace
