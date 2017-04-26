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

auto getSubtree(const Node& tree, const std::string& path) -> const Node&
{
  auto pathSegments = splitPath(path);

  // Traverse branches
  const Node* node = &tree;
  for (int i = 0; i < pathSegments.size(); ++i) {
    try {
      node = &(boost::get<const Branch&>(node)->at(pathSegments[i]));
    }
    catch (const std::out_of_range& e) {
      BOOST_THROW_EXCEPTION(e);
    }
  }

  return *node;
}

auto keyValuesToTree(const std::vector<std::pair<std::string, Leaf>>& pairs) -> Node
{
  Branch treeRoot;

  for (auto& pair : pairs) {
    auto pathSegments = splitPath(pair.first);

    if (pathSegments.empty()) {
      continue;
    }

    // Gets the last segment. This is not a directory but the "key name" (analogous to a filename)
    std::string keyName = pathSegments.back();
    pathSegments.pop_back();

    // Traverses or creates directories
    Branch* node = &treeRoot; // Current node
    for (int i = 0; i < pathSegments.size(); ++i) {
      const auto& directory = pathSegments[i]; // The "directory" to add or get
      if (node->count(directory)) {
        // Key was already present
        if (auto* branch = boost::get<Branch>(&node->at(directory))) {
          // Key was branch, leave alone
        } else {
          // Key was leaf, replace with branch
          (*node)[directory] = Branch();
        }
        node = &(boost::get<Branch>(node->at(directory))); // Set current node to the new Branch
      } else {
        // Key was not yet present
        auto iter = node->emplace(directory, Branch()).first; // Insert Branch and get an iterator to it
        node = &(boost::get<Branch>(iter->second)); // Set current node to the new Branch
      }
    }

    // Finally, we add the value
    node->insert(std::make_pair(keyName, pair.second));
  }

  return treeRoot;
}

void treeToKeyValuesHelper(const Node& node, std::vector<std::pair<std::string, Leaf>>& pairs,
    std::vector<std::string>& pathStack)
{
  auto makeKey = [&pathStack]{
      std::ostringstream stream;
      stream << '/'; // Add leading slash
      for (int i = 0; i < pathStack.size(); ++i) {
        stream << pathStack[i];
        if (i + 1 < pathStack.size()) {
          // Add slash between paths only if we're not at the end
          stream << '/';
        }
      }
      return stream.str();
  };

  Visitor::apply(node,
      [&](const Branch& branch) {
        for (const auto& keyValuePair : branch) {
          pathStack.push_back(keyValuePair.first);
          treeToKeyValuesHelper(keyValuePair.second, pairs, pathStack);
          pathStack.pop_back();
        }
      },
      [&](const Leaf& leaf) {
        pairs.emplace_back(makeKey(), leaf);
      }
  );
}

auto treeToKeyValues(const Node& node) -> const std::vector<std::pair<std::string, Leaf>>
{
  std::vector<std::pair<std::string, Leaf>> pairs;
  std::vector<std::string> pathStack;
  treeToKeyValuesHelper(node, pairs, pathStack);
  return pairs;
}


} // namespace Tree
} // namespace Configuration
} // namespace AliceO2

