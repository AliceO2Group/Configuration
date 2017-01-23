/// \file Tree.h
/// \brief Implementation of the Tree data structure used for getRecursive(), and helper functions
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include "include/Configuration/Tree.h"
#include <string>
#include <map>
#include <boost/throw_exception.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace AliceO2
{
namespace Configuration
{
namespace Tree
{

// Split path into segments (either directories or the name of the key)
// Note: this can be done much more efficiently by just "walking" through the string. But this is easier for now.
auto splitPath(const std::string& path) -> std::vector<std::string>
{
  // Strip leading and trailing '/', or the first/last segment in path will be an empty string
  auto trimmedPath = boost::trim_copy_if(path, boost::algorithm::is_any_of("/ "));
  std::vector<std::string> split;
  if (!trimmedPath.empty()) {
    boost::split(split, trimmedPath, boost::is_any_of("/"));
  }
  return split;
}

auto getSubtree(const Tree::Node& tree, const std::string& path) -> const Tree::Node&
{
  auto pathSegments = splitPath(path);

  // Traverse branches
  const Tree::Node* node = &tree;
  for (int i = 0; i < pathSegments.size(); ++i) {
    try {
      node = &(boost::get<const Tree::Branch&>(node)->at(pathSegments[i]));
    }
    catch (const std::out_of_range& e) {
      BOOST_THROW_EXCEPTION(e);
    }
  }

  return *node;
}

auto keyValuesToTree(const std::vector<std::pair<std::string, Tree::Leaf>>& pairs) -> Tree::Node
{
  Tree::Branch treeRoot;

  for (auto& pair : pairs) {
    auto pathSegments = splitPath(pair.first);

    if (pathSegments.empty()) {
      continue;
    }

    // Gets the last segment. This is not a directory but the "key name" (analogous to a filename)
    std::string keyName = pathSegments.back();
    pathSegments.pop_back();

    // Traverses or creates directories
    Tree::Branch* node = &treeRoot; // Current node
    for (int i = 0; i < pathSegments.size(); ++i) {
      const auto& directory = pathSegments[i]; // The "directory" to add or get
      auto iter = node->emplace(directory, Tree::Branch()).first; // Inserts or gets a TreeMap and gets an iterator to it
      node = &(boost::get<Tree::Branch>(iter->second)); // Move the node to the new TreeMap
    }

    // Finally, we add the value
    node->insert(std::make_pair(keyName, pair.second));
  }

  return treeRoot;
}


} // namespace Tree
} // namespace Configuration
} // namespace AliceO2

