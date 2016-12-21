/// \file Tree.h
/// \brief Definitions of the Tree data structure used for getRecursive(), and helper functions
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#ifndef ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATION_TREE_H_
#define ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATION_TREE_H_

#include <string>
#include <map>
#include <boost/optional.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/lexical_cast.hpp>
#include "Configuration/Visitor.h"

namespace AliceO2
{
namespace Configuration
{
namespace Tree
{

/// Node is a recursive boost::variant. This allows us to model the hierarchy of directories and files, as well as
/// key-value hierarchies.
using Node = boost::make_recursive_variant<
    boost::variant<std::string, int, double, bool>, // Leaf node
    std::map<std::string, boost::recursive_variant_> // Branch node
    >::type;

/// Type for "leaf" nodes in Tree that contain the values
/// The boost::variant allows us to accommodate different value types
using Leaf = Node::types::item; // This refers to the first type of the Tree variant;

/// The map used for branch nodes in Tree
/// It can contain a TreeLeaf or another TreeBranch
using Branch = Node::types::next::item; // This refers to the second type of the Tree variant

/// Helper function to extract a Branch type from the Node variant
inline const Branch& getBranch(const Node& node)
{
  return boost::get<Branch>(node);
}

/// Helper function to extract a Leaf type from the Node variant
inline const Leaf& getLeaf(const Node& node)
{
  return boost::get<Leaf>(node);
}

/// Helper function to convert a Leaf variant to another data type using boost::lexical_cast.
/// If the source and target types are the same, no conversion is performed.
template <class T>
T convert(const Leaf& variant)
{
  if (variant.type() == typeid(T)) {
    // If the variant contains type T, no conversion is needed
    return boost::get<T>(variant);
  } else {
    // Else, try to convert
    try {
      return Visitor::apply<T>(variant,
          [&](const std::string& value) { return boost::lexical_cast<T>(value); },
          [&](int value) { return boost::lexical_cast<T>(value); },
          [&](bool value) { return boost::lexical_cast<T>(value); },
          [&](double value) { return boost::lexical_cast<T>(value); });
    }
    catch (const boost::bad_lexical_cast& e) {
      BOOST_THROW_EXCEPTION(e);
    }
  }
}

/// Helper function to extract and convert a Leaf type from the Node variant
template <class T>
T get(const Node& node)
{
  return convert<T>(getLeaf(node));
}

/// Helper function to extract and convert a Leaf type from a Branch
template <class T>
T get(const Branch& branch, const std::string& key)
{
  return convert<T>(getLeaf(branch.at(key)));
}

/// Helper function to extract and convert a Leaf type from a Branch
template <class T>
T get(const Node& node, const std::string& key)
{
  return get<T>(getBranch(node), key);
}

/// Helper function to extract and convert a Leaf type from a Branch
template <class T>
boost::optional<T> getOptional(const Branch& node, const std::string& key)
{
  auto iter = node.find(key);
  if (iter != node.end()) {
    return get<T>(iter->second);
  } else {
    return {};
  }
}

/// Helper function to extract and convert a Leaf type from a Branch
template <class T>
boost::optional<T> getOptional(const Node& node, const std::string& key)
{
  return getOptional<T>(getBranch(node), key);
}


/// Prints a tree
inline void printTree(const Node& node, std::ostream& stream, int level = 0)
{
  // The visitor is applied recursively to the tree.
  // The appropriate lambda is called depending on what type is contained in the node variant
  Visitor::apply(node,
      [&](const Branch& branch) {
        stream << '\n';
        for (const auto& keyValuePair : branch) {
          stream << std::string(level*2, ' '); // Indentation
          stream << keyValuePair.first << " -> "; // Print name of the branch
          printTree(keyValuePair.second, stream, level + 1); // Recursive step
        }
      },
      [&](const Leaf& leaf) {
        // We can again visit each data type if we need
        Visitor::apply(leaf,
            [&](const std::string& value) { stream << "String: " << value; },
            [&](int value) { stream << "Int:    " << value; },
            [&](bool value) { stream << "Bool:   " << value; },
            [&](double value) { stream << "Double: " << value; });
        // Or we could do it like this:
        // stream << convert<std::string>(leaf);
        stream << '\n';
      }
  );
}

/// Split path into segments (either directories or the name of the key)
auto splitPath(const std::string& path) -> std::vector<std::string>;

/// Gets a subtree based on a path string
auto getSubtree(const Tree::Node& node, const std::string& path) -> const Tree::Node&;

/// Converts key-value pairs into a tree
auto keyValuesToTree(const std::vector<std::pair<std::string, Tree::Leaf>>& kvs) -> Tree::Node;

} // namespace Tree
} // namespace Configuration
} // namespace AliceO2

#endif /* ALICEO2_CONFIGURATION_INCLUDE_CONFIGURATION_TREE_H_ */
