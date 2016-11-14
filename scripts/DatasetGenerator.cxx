/// \file DatasetGenerator.cxx
///
/// \brief Script for generating CSV datasets for the purpose of testing the Configuration module
///
/// \author Pascal Boeschoten
///

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

using std::cout;
using std::endl;
using KeyValueMap = std::map<std::string, std::string>;
namespace po = boost::program_options;

constexpr int optionValueLengthDefault = 100;
constexpr int optionValueLengthMinimum = 10;
constexpr int optionPairsPerLevelDefault = 5;

/// Create string that represents the value in a key-value pair
std::string generateValueString(int value, int length)
{
  std::stringstream stringStream;
  stringStream << "value" << std::setw(length - 5) << std::setfill('0') << value;
  return stringStream.str();
}

KeyValueMap generateFragmented(int numberOfPairs, int valueLength, std::string keyPrefix)
{
  KeyValueMap keyValueMap;

  for (int i = 0; i < numberOfPairs; ++i) {
    keyValueMap.emplace(keyPrefix + "/key" + boost::lexical_cast<std::string>(i), generateValueString(i, valueLength));
  }

  return keyValueMap;
}

KeyValueMap generateBlob(int numberOfPairs, int valueLength, std::string keyPrefix)
{
  KeyValueMap keyValueMap;
  std::stringstream stringStream;

  for (int i = 0; i < numberOfPairs; ++i) {
    stringStream << "key" << i << '=' << generateValueString(i, valueLength) << '|';
  }

  keyValueMap.emplace(keyPrefix, stringStream.str());
  return keyValueMap;
}

/// Recursive helper function for generateTree()
void _generateTreeHelper(const int numberOfPairs, const int valueLength, int& keyValueCounter,
    const int neededDepth, const int depthCounter, const std::string currentDirectoryKey, const int maxParametersPerDirectory,
    KeyValueMap& keyValueMap)
{
  if (depthCounter > neededDepth) {
    return;
  }

  if (keyValueCounter >= numberOfPairs) {
    return;
  }

  int addedParameters = 0;
  while (keyValueCounter < numberOfPairs && addedParameters < maxParametersPerDirectory) {
    keyValueMap.emplace(
        currentDirectoryKey + "/key" + boost::lexical_cast<std::string>(keyValueCounter),
        generateValueString(keyValueCounter, valueLength));

    keyValueCounter++;
    addedParameters++;
  }

  _generateTreeHelper(numberOfPairs, valueLength, keyValueCounter, neededDepth, depthCounter + 1,
      currentDirectoryKey + "/dirA", maxParametersPerDirectory, keyValueMap);

  _generateTreeHelper(numberOfPairs, valueLength, keyValueCounter, neededDepth, depthCounter + 1,
      currentDirectoryKey + "/dirB", maxParametersPerDirectory, keyValueMap);
}

KeyValueMap generateTree(int numberOfPairs, int valueLength, std::string keyPrefix, int optionPairsPerLevel)
{
  KeyValueMap parameterMap;
  std::string currentDirectoryKey = keyPrefix;

  // Figure out the necessary tree depth (there's a more clever way to do it)
  auto findDepth = [](int nParameters, int paramsPerLevel){
    int depth = 0;
    int maxElements = 0;
    for (;;) {
      maxElements += ::pow(2, depth) * paramsPerLevel;
      if (nParameters <= maxElements) {
        return depth;
      }
      depth++;
    }
  };

  int maxPairsPerDirectory = optionPairsPerLevel;
  int neededDepth = findDepth(numberOfPairs, maxPairsPerDirectory);
  int pairsCounter = 0;
  int depthCounter = 0;

  _generateTreeHelper(numberOfPairs, valueLength, pairsCounter, neededDepth, depthCounter,
      currentDirectoryKey, maxPairsPerDirectory, parameterMap);

  return parameterMap;
}

void printKeyValueMap(const KeyValueMap& keyValueMap)
{
  for (auto& keyValue: keyValueMap) {
    cout << keyValue.first << ',' << keyValue.second << '\n';
  }
}

int main(int argc, char** argv)
{
  int optionNumberOfPairs;
  int optionValueLength;
  std::string optionStructure;
  int optionPairsPerLevel;
  std::string optionKeyPrefix;

  po::options_description description("Allowed options");
  description.add_options()
      ("help",
          "Produce help message")
      ("num-pairs,n", po::value(&optionNumberOfPairs)->required(),
          "Number of key-value pairs")
      ("value-length,l", po::value(&optionValueLength)->default_value(optionValueLengthDefault),
          "Size of each value in amount of characters")
      ("structure,s", po::value(&optionStructure)->required(),
          "Structure of data set: fragmented, blob, flat, tree")
      ("pairs-per-level,p", po::value(&optionPairsPerLevel)->default_value(optionPairsPerLevelDefault),
          "For nested structures, the amount of key-value pairs per level")
      ("key-prefix,k", po::value(&optionKeyPrefix),
          "Prefix for keys");

  auto printHelp = [&]() {
    cout << "Dataset Generator\n"
    << "  Script for generating test datasets for the Configuration module.\n"
    << "  Output is in CSV format with every line containing a key and value\n"
    << "  separated by a comma.\n\n"
    << description << "\n"
    << "Example:\n"
    << "  ./DatasetGenerator -n 20 -l 10 -p 3 -s tree > tree_n20_l10_p3.csv\n";
  };

  auto printErrorAndHelp = [&](std::string errorMessage) {
    cout << "ERROR: " << errorMessage << "\n\n";
    printHelp();
  };

  po::variables_map variablesMap;

  try {
    po::store(po::parse_command_line(argc, argv, description), variablesMap);
    po::notify(variablesMap);
  } catch (std::exception& e) {
    printErrorAndHelp(e.what());
    return 1;
  }

  if (variablesMap.count("help")) {
    printHelp();
    return 1;
  }

  if (optionNumberOfPairs < 1) {
    printErrorAndHelp("option '--num-pairs' must be >= 1");
    return 1;
  }

  if (optionValueLength < optionValueLengthMinimum) {
    printErrorAndHelp("option '--size' below minimum of 10");
    return 1;
  }

  if (optionPairsPerLevel < 1) {
    printErrorAndHelp("option '--pairs-per-level' must be >= 1");
    return 1;
  }

  if (variablesMap.count("key-prefix")) {
    optionKeyPrefix = variablesMap["key-prefix"].as<std::string>();
  } else {
    std::stringstream stringStream;
    stringStream << "/test/" << optionStructure << optionNumberOfPairs;
    optionKeyPrefix = stringStream.str();
  }

  KeyValueMap keyValueMap;

  if (optionStructure == "fragmented") {
    keyValueMap = generateFragmented(optionNumberOfPairs, optionValueLength, optionKeyPrefix);
  } else if (optionStructure == "blob") {
    keyValueMap = generateBlob(optionNumberOfPairs, optionValueLength, optionKeyPrefix);
  } else if (optionStructure == "flat") {
    // Note that this is actually identical to fragmented, except with a different prefix
    // Keeping it for now to "support" the terminology that's been used already
    keyValueMap = generateFragmented(optionNumberOfPairs, optionValueLength, optionKeyPrefix);
  } else if (optionStructure == "tree") {
    keyValueMap = generateTree(optionNumberOfPairs, optionValueLength, optionKeyPrefix, optionPairsPerLevel);
  } else {
    printErrorAndHelp("invalid '--structure' argument");
    return 1;
  }

  printKeyValueMap(keyValueMap);
}
