/// \file Benchmark.cxx
/// \brief Command-line utility for benchmarking configuration backends.
///
/// A rough port of old benchmarking code to use the Configuration library
///
/// \author Pascal Boeschoten (pascal.boeschoten@cern.ch)

#include <unistd.h>
#include <limits.h>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Configuration/ConfigurationFactory.h"

namespace {

#define PARAM_MODE_SEPARATE "separate"
#define PARAM_MODE_COMBINED "combined"
#define PARAM_MODE_FLAT "flat"
#define PARAM_MODE_TREE "tree"

using namespace AliceO2::Configuration;
namespace po = boost::program_options;
using ParameterMap = std::unordered_map<std::string, std::string>;
using KeySet = std::unordered_set<std::string>;

struct Options
{
    std::vector<std::string> serverUris;
    std::string logDirectory;
    std::string logId;
    std::string parameterStructure;
    int parameterNumber;
    bool skipWait;
    bool skipCheckValues;
    bool put;
    bool printParams;
    bool help;
    bool verbose;
};

bool sVerbose = false;

auto log() -> std::ostream&
{
  static std::ofstream deadStream; // Unopened stream is essentially a '/dev/null'
  return sVerbose ? std::cout : deadStream;
}

auto getOptions(int argc, char** argv) -> Options
{
  Options options;
  std::string serverUris;

  auto optionsDescription = po::options_description("Options");
  optionsDescription.add_options()
      ("help",
          po::bool_switch(&options.help)->default_value(false),
          "Print help")
      ("verbose",
          po::bool_switch(&options.verbose)->default_value(false),
          "Verbose output")
      ("param-structure",
          po::value<std::string>(&options.parameterStructure)->default_value(PARAM_MODE_SEPARATE),
          "Parameter structure ['" PARAM_MODE_SEPARATE "', '" PARAM_MODE_COMBINED "', '" PARAM_MODE_FLAT "', '"
          PARAM_MODE_TREE "']")
      ("param-n",
          po::value<int>(&options.parameterNumber)->default_value(1),
          "Number of parameters")
      ("uri",
          po::value<std::string>(&serverUris),
          "Server URI. Can give multiple separated by comma. Get mode will 'randomly' pick server based on PID, "
          "put mode will put to all servers")
      ("log-dir",
          po::value<std::string>(&options.logDirectory),
          "Output directory for result logs. Log format: log ID, PID, start time, end time, delta time")
      ("log-id",
          po::value<std::string>(&options.logId),
          "Output ID for result logs. If not specified, the hostname will be used")
      ("skip-wait",
          po::bool_switch(&options.skipWait),
          "Skip wait until simulated start")
      ("skip-check",
          po::bool_switch(&options.skipCheckValues),
          "Skip checking values returned form server")
      ("put",
          po::bool_switch(&options.put),
          "Put to server instead of get, also skips wait")
      ("print-params",
          po::bool_switch(&options.printParams),
          "Print the parameter data in csv format and exit");

  auto map = po::variables_map();
  po::store(po::parse_command_line(argc, argv, optionsDescription), map);
  po::notify(map);

  if (options.help) {
    std::cout << optionsDescription << '\n';
  }

  if (options.logId.empty()) {
    char hostname[HOST_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    options.logId = hostname;
  }

  boost::split(options.serverUris, serverUris, boost::is_any_of(","), boost::token_compress_on);

  return options;
}

void waitUntilNextInterval()
{
  using std::chrono::system_clock;

  // Get current time
  auto now = system_clock::to_time_t(system_clock::now());
  auto currentTime = *std::localtime(&now);

  log() << "Current time " << currentTime.tm_hour << ':' << currentTime.tm_min << ':' << currentTime.tm_sec << '\n';

  // Set time for next 1 minute and 10 seconds
  auto nextTime = currentTime;

  if (nextTime.tm_sec < 10) {
    nextTime.tm_sec = 10;
  } else {
    nextTime.tm_min = nextTime.tm_min + 1;
    nextTime.tm_sec = 10;
  }

  log() << "Sleeping until " << nextTime.tm_hour << ':' << nextTime.tm_min << ':' << nextTime.tm_sec << '\n';

  // Sleep until next minute
  std::this_thread::sleep_until(system_clock::from_time_t(mktime(&nextTime)));
}

std::string flatParameterPath(int nParameters)
{
  return "/test/flat" + boost::lexical_cast<std::string>(nParameters);
}

std::string treeParameterPath(int nParameters)
{
  return "/test/tree" + boost::lexical_cast<std::string>(nParameters);
}

std::string makeValue(int number)
{
  std::stringstream stringstream;
  stringstream << "value" << std::setw(95) << std::setfill('0') << number;
  return stringstream.str();
}

int checkReturnedParameters(ParameterMap& generatedMap, ParameterMap& returnedMap)
{
  int mismatches = 0;

  if (generatedMap.size() != returnedMap.size()) {
    log() << "Mismatch of size"
        << " generated:" << generatedMap.size()
        << " returned:" << returnedMap.size() << '\n';
  }

  for (auto& parameter : generatedMap) {
    if (returnedMap.count(parameter.first) == 0) {
      // They key does not exist in the returned list
      mismatches++;
      log() << "Mismatch for key:" << parameter.first
          << " not found in returned list\n";
      continue;
    }

    if (returnedMap[parameter.first] != parameter.second) {
      // The values are not identical
      mismatches++;
      log() << "Mismatch for key:" << parameter.first
          << " expected:" << parameter.second
          << " returned:" << returnedMap[parameter.first] << '\n';
      continue;
    }
  }

  return mismatches;
}

/// Creates a list of parameters and values
///
/// The test keys and values are:
/// /test/key[0...nParams - 1] -> [0...nParams - 1]
ParameterMap createParameterMapSeparate(int nParams)
{
  ParameterMap parameterMap;
  std::string keyPrefix = "/test/separate/key";
  int pathMin = 0;
  int pathMax = nParams - 1;

  for (int i = pathMin; i <= pathMax; ++i) {
    parameterMap.emplace(keyPrefix + boost::lexical_cast<std::string>(i), makeValue(i));
  }

  return parameterMap;
}

/// Creates a ParameterMap with a single entry that combines multiple parameters
/// Uses 16 characters per parameter
ParameterMap createParameterMapCombined(int nParams)
{
  ParameterMap parameterMap;
  std::stringstream stringstream;

  for (int i = 0; i < nParams; ++i) {
    stringstream << "key" << i << "=value" << std::setw(95) << std::setfill('0') << i << '|';
  }

  parameterMap.emplace("/test/combined/key" + boost::lexical_cast<std::string>(nParams), stringstream.str());
  return parameterMap;
}

ParameterMap createParameterMapFlat(int nParameters)
{
  ParameterMap parameterMap;
  std::string pathPrefix = flatParameterPath(nParameters) + "/";

  for (int i = 0; i < nParameters; ++i) {
    parameterMap.emplace(pathPrefix + "key" + boost::lexical_cast<std::string>(i), makeValue(i));
  }

  return parameterMap;
}

/// Recursive helper function for createParameterMapTree()
void _createParameterMapTreeRecursive(
    const int nParameters,
    int& currentParameters,
    const int neededDepth,
    int currentDepth,
    const std::string currentDirKey,
    const int maxParametersPerDirectory,
    ParameterMap& parameterMap)
{
  if (currentDepth > neededDepth) {
    return;
  }

  if (currentParameters >= nParameters) {
    return;
  }

  //cout << currentDirKey << " depth=" << currentDepth << endl;

  int addedParameters = 0;
  while (currentParameters < nParameters && addedParameters < 5) {
    parameterMap.emplace(currentDirKey + "/key" + boost::lexical_cast<std::string>(currentParameters),
        makeValue(currentParameters));

    currentParameters++;
    addedParameters++;

    //for (int i = 0; i < (currentDepth + 1); i++) { cout << "-"; }
    //cout << p.key << " -> " << p.expectedValue << endl;
  }

  _createParameterMapTreeRecursive(nParameters, currentParameters, neededDepth, currentDepth + 1,
      currentDirKey + "/dirA", maxParametersPerDirectory, parameterMap);

  _createParameterMapTreeRecursive(nParameters, currentParameters, neededDepth, currentDepth + 1,
      currentDirKey + "/dirB", maxParametersPerDirectory, parameterMap);
}

ParameterMap createParameterMapTree(int nParameters)
{
  ParameterMap parameterMap;
  std::string currentDirKey = treeParameterPath(nParameters);

  int currentParameters = 0;

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

  int maxParametersPerDirectory = 5;
  int neededDepth = findDepth(nParameters, maxParametersPerDirectory);
  int currentDepth = 0;

  //cout << "Creating tree, neededDepth=" << neededDepth << endl;

  _createParameterMapTreeRecursive(nParameters, currentParameters, neededDepth, currentDepth, currentDirKey,
      maxParametersPerDirectory, parameterMap);

  return parameterMap;
}

void putParametersToServer(ConfigurationInterface* configuration, const ParameterMap& parameterMap)
{
  log() << "Putting key-values: \n";
  for (const auto& kv : parameterMap) {
    log() << " - " << kv.first << " -> " << kv.second << '\n';
    configuration->putString(kv.first, kv.second);
  }
//  globalRetries += backend->totalRetries;
}

ParameterMap getParametersFromServer(ConfigurationInterface* configuration, const ParameterMap& keys)
{
  ParameterMap map;
  log() << "Getting keys: \n";
  for (const auto& kv : keys) {
    log() << " - " << kv.first << '\n';
    map.emplace(kv.first, configuration->getString(kv.first).value());
  }
//  globalRetries += backend->totalRetries;
  return map;
}

ParameterMap getParametersFromServerRecursive(ConfigurationInterface* configuration, std::string key)
{
  ParameterMap map;
  log() << "Getting recursive: " << key << '\n';
  Tree::Node node = configuration->getRecursive(key);
  auto keyValues = Tree::treeToKeyValues(node);
  for (const auto& kv : keyValues) {
    map.emplace(key + kv.first, Tree::convert<std::string>(kv.second));
  }
//  globalRetries += backend->totalRetries;
  return map;
}

// Abstract base class for handling different parameter structures: how to put them, get them and check the results
class ParameterHandler
{
  public:
    virtual ~ParameterHandler()
    {
    }

    virtual void put(ConfigurationInterface* configuration, int nParameters)
    {
      auto parameterMap = createParameterMap(nParameters);
      putParametersToServer(configuration, parameterMap);
    }

    virtual void get(ConfigurationInterface* configuration, int nParameters)
    {
      generatedMap = createParameterMap(nParameters);
      returnedMap = getParametersFromServer(configuration, generatedMap);
    }

    virtual int check()
    {
      return checkReturnedParameters(generatedMap, returnedMap);
    }

    virtual ParameterMap createParameterMap(int nParameters) = 0;

    ParameterMap generatedMap;
    ParameterMap returnedMap;
};

/// One query per parameter
class SeparateParameterHandler: public ParameterHandler
{
  private:
    virtual ParameterMap createParameterMap(int nParameters)
    {
      return createParameterMapSeparate(nParameters);
    }
};

/// One query per process, parameters combined into one string
class CombinedParameterHandler: public ParameterHandler
{
  public:
    virtual ParameterMap createParameterMap(int nParameters)
    {
      return createParameterMapCombined(nParameters);
    }
};

/// One query per process, parameters in one flat directory
class FlatParameterHandler: public ParameterHandler
{
  public:
    virtual void get(ConfigurationInterface* configuration, int nParameters)
    {
      generatedMap = createParameterMap(nParameters);
      returnedMap = getParametersFromServerRecursive(configuration, flatParameterPath(nParameters));
    }

    virtual ParameterMap createParameterMap(int nParameters)
    {
      return createParameterMapFlat(nParameters);
    }
};

/// One query per process, parameters in tree directory structure
class TreeParameterHandler: public ParameterHandler
{
  public:
    virtual void get(ConfigurationInterface* configuration, int nParameters)
    {
      generatedMap = createParameterMap(nParameters);
      returnedMap = getParametersFromServerRecursive(configuration, treeParameterPath(nParameters));
    }

    virtual ParameterMap createParameterMap(int nParameters)
    {
      return createParameterMapTree(nParameters);
    }
};

auto getConfiguration(const Options& options) -> std::unique_ptr<ConfigurationInterface>
{
  auto selectUri = [&]{
    if (options.serverUris.empty()) {
      throw std::runtime_error("No server URIs specified");
    } else if (options.serverUris.size() == 1) {
      log() << "Server URI: " << options.serverUris.at(0) << '\n';
      return options.serverUris.at(0);
    } else {
      auto pid = ::getpid();
      const auto& serverUri = options.serverUris.at(pid % options.serverUris.size());
      log() << "Used PID " << pid << " to select 'round-robin' server URI: " << serverUri << '\n';
      return serverUri;
    }
  };

  return ConfigurationFactory::getConfiguration(selectUri());
}

struct ParameterSpecification
{
    struct SingleGet
    {
        std::string key;
        std::string value;
    };

    std::vector<SingleGet> singleGets;
    std::vector<std::string> separatePaths;
    std::vector<std::string> recursivePaths;
};

auto getParameterHandler(const Options& options) -> std::unique_ptr<ParameterHandler>
{
  if (options.parameterStructure == PARAM_MODE_SEPARATE) {
    return std::make_unique<SeparateParameterHandler>();
  } else if (options.parameterStructure == PARAM_MODE_COMBINED) {
    return std::make_unique<CombinedParameterHandler>();
  } else if (options.parameterStructure == PARAM_MODE_FLAT) {
    return std::make_unique<FlatParameterHandler>();
  } else if (options.parameterStructure == PARAM_MODE_TREE) {
    return std::make_unique<TreeParameterHandler>();
  } else {
    throw std::runtime_error("invalid 'mode' option");
  }
}

void printMapCsv(const ParameterMap& map)
{
  for (auto& kv : map) {
    log() << kv.first << "," << kv.second << "\n";
  }
}

void logStats(std::string directory, std::string id, std::chrono::high_resolution_clock::time_point startTime,
    std::chrono::high_resolution_clock::time_point endTime)
{
  int pid = ::getpid();
  auto toMillis = [](auto t){ return std::chrono::duration_cast<std::chrono::milliseconds>(t).count(); };

  std::stringstream ss;
  ss << id << ','
      << pid << ','
      << toMillis(startTime.time_since_epoch()) << ','
      << toMillis(endTime.time_since_epoch()) << ','
      << toMillis(endTime - startTime) << '\n';

  FILE* file;
  std::string path = directory + "/result_" + id + ".csv";
  log() << "Logging to " << path << '\n';
  // Open in append mode
  file = fopen (path.c_str(), "a");
  // Disable buffering to prevent concurrency issues
  setvbuf(stdout, NULL, _IONBF, 0);
  fputs(ss.str().c_str(), file);
  fclose(file);
}

} // Anonymous namespace

int main(int argc, char** argv)
{
  const Options options = getOptions(argc, argv);
  sVerbose = options.verbose;

  if (options.help) {
    return 0;
  }

  auto parameterHandler = getParameterHandler(options);

  if (options.printParams) {
    // Print data set and exit
    log() << "Printing parameters\n";
    printMapCsv(parameterHandler->createParameterMap(options.parameterNumber));
  }
  else if (options.put) {
    // Put mode
    log() << "Putting '" << options.parameterNumber << "' parameters to servers ";
    for (const auto& uri : options.serverUris) {
      log() << "'" << uri << "' ";
    }
    log() << '\n';

    for (const auto& uri : options.serverUris) {
      auto configuration = ConfigurationFactory::getConfiguration(uri);
      parameterHandler->put(configuration.get(), options.parameterNumber);
    }
  }
  else {
    if (options.logDirectory.empty()) {
      throw std::runtime_error("Must specify log directory with '--log-dir' option");
    }

    // Wait for next minute if required
    if (!options.skipWait) {
      log() << "Waiting until next interval\n";
      waitUntilNextInterval();
    }

    // Get parameters from server
    try {
      log() << "Getting from server\n";
      auto configuration = getConfiguration(options);
      auto startTime = std::chrono::high_resolution_clock::now();
      parameterHandler->get(configuration.get(), options.parameterNumber);
      auto endTime = std::chrono::high_resolution_clock::now();
      logStats(options.logDirectory, options.logId, startTime, endTime);
    } catch (std::exception& e) {
      log() << "FATAL: " << e.what() << '\n';
      return 1;
    }

    if (!options.skipCheckValues) {
      // Verify returned values
      log() << "Checking parameters\n";
      int mismatches = parameterHandler->check();
      if (mismatches > 0) {
        std::cout << "Mismatches found: " << mismatches << '\n';
      }

      log() << "# Generated\n";
      printMapCsv(parameterHandler->generatedMap);
      log() << "# Returned\n";
      printMapCsv(parameterHandler->returnedMap);
    }
  }
}

