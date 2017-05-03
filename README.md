# Configuration module
The Configuration module provides a way to access Configuration data from various backends in a uniform way.
Users get an interface to backends by providing a URI to the ConfigurationFactory class, which returns an implementation 
of ConfigurationInterface.
Values can be retrieved from the interface one-by-one using simple getters, or multiple at a time using `getRecursive()`.
The `getRecursive()` function returns a tree-like data structure, which models the hierarchy of directories or paths.


# Backends
This section provides some basic information on the available backends and their dependencies  

## File
* Reads .ini style files
* No dependencies

## JSON
* Reads .json files
* Requires RapidJSON

## etcd v2
* Interface to etcd v2 API
* Requires RapidJSON

## etcd v3
* Interface to etcd v3 API
* Requires Protobuf and gRPC 

## Consul
* Interface to Consul API
* Requires ppconsul
* Work in progress


# Examples
Basic usage:

~~~
std::string uri = "etcd://localhost:2379"; 
std::unique_ptr<ConfigurationInterface> conf = ConfigurationFactory::getConfiguration(uri);
conf->put<int>("/my_dir/my_key", 123);
int value = conf->get<int>("/my_dir/my_key");
~~~

There are more usage examples in the file `test/TestExamples.cxx`. 
The unit tests may also be useful as examples.


# Command line utilities
The library includes some simple command line utilities that can be used to interact with backends.
* `configuration-put` for putting values
* `configuration-get` for getting values
* `configuration-copy` for copying values
For usage, refer to their respective `--help` options.

Because they depend on Common, and Common is not yet in alibuild, the compilation of these utilities is disabled by
default. To enable them, add `-DBUILD_UTILITIES=TRUE` to the cmake parameters.


# Installation

## aliBuild installation
~~~
cd $HOME/alice
aliBuild init Configuration@master # checkout the code of
                                   # Configuration, branch master
aliDoctor Configuration            # To make sure that we are good.
aliBuild --defaults o2 build Configuration
alienv load Configuration/latest

# At this stage, modify the project.

# Re-build by doing either
aliBuild --defaults o2 build Configuration
# or (faster compilation)
cd sw/BUILD/Configuration-latest/Configuration
make -j install
~~~
For more information: https://alisw.github.io/alibuild/o2-tutorial.html

## Manual installation

This section provides instructions for manually installing the Configuration module and its dependencies

### Protocol Buffer & gRPC
Needed for etcd-v3 back-end.

~~~
wget https://github.com/google/protobuf/releases/download/v3.0.0/protobuf-cpp-3.0.0.tar.gz
tar zxf protobuf-cpp-3.0.0.tar.gz
cd protobuf-3.0.0
./configure
make -j
make install
~~~

~~~
git clone https://github.com/grpc/grpc
cd grpc
git submodule update --init
make -j
make install
~~~

### RapidJSON
~~~
git clone https://github.com/miloyip/rapidjson.git
cd rapidjson; mkdir build; cd build
cmake ..
make -j
sudo make install
~~~

### libcurl
Should be available in your OS's package manager, or else: https://curl.haxx.se/download.html

### ppconsul
~~~
git clone https://github.com/oliora/ppconsul.git
cd ppconsul 
git checkout 8ade80d0528b563d4b58bc4f09815fc1e3d5be19
mkdir build; cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make -j
cp output/*.so /usr/local/lib/
cp ../include/* /usr/local/include/
~~~

### etcd
Note: this is not a compile-time dependency, just an optional runtime-dependency
Follow instructions on: `https://github.com/coreos/etcd/releases/`
Or use Docker: 
~~~
docker run --name=etcd --net=host quay.io/coreos/etcd:v3.0.14
~~~

### Configuration
~~~
git clone https://github.com/AliceO2Group/Configuration.git
cd Configuration; mkdir build; cd build
cmake ..
make -j
~~~
(For more information: https://github.com/henszey/etcd-browser)

## Doxygen

`make doc` will generate the doxygen documentation.
To access the resulting documentation, open doc/html/index.html in your
build directory. To install the documentation when calling `make install`
turn on the variable `DOC_INSTALL`.
