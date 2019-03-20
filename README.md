[![Travis Status](https://travis-ci.com/AliceO2Group/Configuration.svg?branch=master)](https://travis-ci.com/AliceO2Group/Configuration)
[![JIRA](https://img.shields.io/badge/JIRA-issues-blue.svg)](https://alice.its.cern.ch/jira/projects/OCONF)

# Configuration module
The Configuration module provides a way to access Configuration data from various backends in a uniform way.
Users get an interface to backends by providing a URI to the ConfigurationFactory class, which returns an implementation
of ConfigurationInterface.
Values can be retrieved from the interface one-by-one using simple getters, or multiple at a time using `getRecursive()`.
The `getRecursive()` function returns a tree-like data structure, which models the hierarchy of directories or paths.

## Installation
To install without `aliBuild` follow [these instructions](doc/ManualInstallation.md).

### aliBuild installation
Follow [this](https://alice-doc.github.io/alice-analysis-tutorial/building/) tutorial to install `aliBuild`.

Install `Configuration` module:
```
aliBuild build Configuration --defaults o2
alienv load Configuration/latest
```

##### Development
If you want to make changes to the source code initialise the development package first:
```
aliBuild init Configuration@master
```
and then use `make` to recompile
```
cd sw/BUILD/Configuration-latest/Configuration
make -j install
```

## Instance and backends
The configuration instance can be requested form the factory based on provided URI:
```cpp
#include <Configuration/ConfigurationFactory.h>
using namespace o2::configuration;

std::unique_ptr<ConfigurationInterface> = ConfigurationFactory::getConfiguration("backend://[host][:port][/path]");
```

The URI is constructed based on the table below:

| Backend name | URI backned  | Host  | Port | Path  | Dependency |
| ------------ |:------------:|:-----:|:----:|:-----:|-----------:|
| INI file     | `ini://`     | -     | - | File path | - |
| JSON file    | `json://`    | -     | - | File path | - |
| Consul       | `consul://`  | Server's hostname | Server's port | - | [ppconsul](https://github.com/oliora/ppconsul) |



## Getting values
Use `.` as path separator.

#### Getting a value
Simply use templated `get` method and provide `path` as parameter:
```cpp
auto conf = ConfigurationFactory::getConfiguration("ini:///temp/config.ini");
int value = conf->get<int>("my_dir.my_key");
```
#### Using prefix
If you need to `get` multiple values from a single node consider using `setPrefix`:
```cpp
auto conf = ConfigurationFactory::getConfiguration("json:///temp/config.json");
conf->setPrefix("my_dir");
int value = conf->get<int>("my_key");
```

#### Managing failures
When the value under requested path does not exist use one of the following ways to handle it.

1. Catch an exception

```cpp
auto conf = ConfigurationFactory::getConfiguration("ini:///temp/config.ini");
try {
  conf->get<int>("my_dir.my_wrong_key");
} catch (std::runtime_error&) { ... }

```

2. Provide default value as a second parameter to `get` method

```cpp
auto conf = ConfigurationFactory::getConfiguration("ini:///temp/config.ini");
int value = conf->get<int>("my_dir.my_wrong_key", 321);
```

#### Getting a subtree of values
When tree of values or flat key-value map is needed:
```cpp
auto conf = ConfigurationFactory::getConfiguration("consul://localhost:8500");

// Get a tree
boost::property_tree::ptree subTree = conf->getRecursive("my_dir");
subTree.get<int>("my_key");

// Get flat key-value map
std::unordered_map<std::string, std::string> map = conf->getRecursiveMap("my_dir");
map["my_key"];
```

## Putting values
Putting values in currently supported only by Consul backend.
```cpp
auto conf = ConfigurationFactory::getConfiguration("consul://localhost:8500");
conf->put<int>("my_dir.my_key", 123);
```

## Consul server setup
See [detailed instructions](doc/Consul.md).
