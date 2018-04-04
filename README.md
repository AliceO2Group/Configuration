# Configuration module
The Configuration module provides a way to access Configuration data from various backends in a uniform way.
Users get an interface to backends by providing a URI to the ConfigurationFactory class, which returns an implementation 
of ConfigurationInterface.
Values can be retrieved from the interface one-by-one using simple getters, or multiple at a time using `getRecursive()`.
The `getRecursive()` function returns a tree-like data structure, which models the hierarchy of directories or paths.

# Reporting Bugs

In order to report bugs or feature requests, please use the associated JIRA project, <https://alice.its.cern.ch/jira/projects/OCONF>.

# Backends
This section provides some basic information on the available backends and their dependencies  

## File
* Reads .ini style files
* No dependencies

## JSON
* Reads .json files
* Requires RapidJSON

## Consul
* Interface to Consul API
* Requires ppconsul
* Work in progress


# Examples
Basic usage:

~~~
std::string uri = "consul://localhost:8500"; 
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


# Installation
First make sure you have the devtoolset-6 GCC
~~~
sudo yum -y install centos-release-scl
sudo yum-config-manager --enable rhel-server-rhscl-7-rpms
sudo yum -y install devtoolset-6
source /opt/rh/devtoolset-6/enable  # Do this every session, or add to your ~/.bashrc 
~~~

## aliBuild installation

~~~
# Install alibuild
sudo yum -y install python2-pip ncurses-devel bzip2-devel libX11-devel libXpm-devel libXext-devel \
    libXft-devel mesa-libGLU-devel perl-ExtUtils-Embed perl-Thread-Queue openssl-devel libxml2-devel libcurl-devel git \
    bison flex
sudo pip install alibuild==1.4.0
mkdir -p $HOME/alice
cd $HOME/alice

# Build
aliBuild init Configuration@master
aliDoctor Configuration
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

First get some additional prerequisite packages:
~~~
sudo yum -y install wget git cmake cmake3 autoconf automake
~~~

### RapidJSON
Needed for JSON back-end.
~~~
cd /tmp
git clone https://github.com/miloyip/rapidjson.git
cd rapidjson
git checkout v1.1.0
mkdir build; cd build
cmake ..
make -j
sudo make install
~~~

### libcurl
Should be available in your OS's package manager, or else: https://curl.haxx.se/download.html
For CC7:
~~~
sudo yum -y install curl-devel
~~~

### Boost
Note: ppconsul requires 1.55 or newer
~~~
cd /tmp
wget http://dl.bintray.com/boostorg/release/1.65.0/source/boost_1_65_0.tar.gz
tar zxf boost_1_65_0.tar.gz
cd boost_1_65_0
./bootstrap.sh
./b2 \
  --without-container       \
  --without-context         \
  --without-coroutine       \
  --without-graph           \
  --without-graph_parallel  \
  --without-locale          \
  --without-math            \
  --without-mpi             \
  --without-python          \
  --without-wave            
# This is a good time to fetch a beverage
sudo ./b2 install
~~~ 

### ppconsul
~~~
cd /tmp
git clone https://github.com/oliora/ppconsul.git
cd ppconsul 
git checkout 8ade80d0528b563d4b58bc4f09815fc1e3d5be19
mkdir build; cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make -j
sudo cp output/*.so /usr/local/lib/
sudo cp -r ../include/* /usr/local/include/
~~~

### Configuration
~~~
cd /tmp
git clone https://github.com/AliceO2Group/Configuration.git
cd Configuration; mkdir build; cd build
cmake3 ..
make -j
sudo make install
~~~

## Backend server setup
First install Docker
~~~
sudo yum -y install docker
~~~

### Consul
Local-only development setup
~~~
sudo docker run -d --name=dev-consul --net=host consul:0.7.5 \
  consul agent -dev -ui
~~~

Externally visible
~~~
sudo docker run -d --name=consul --net=host consul:0.7.5 \
  consul agent -ui -server -bind=<external ip> -client=<external ip> -bootstrap-expect=1
~~~

For more information: 
https://hub.docker.com/_/consul/

## GUI
There is currently no generalized Configuration GUI, although this feature is planned.
For now, we recommend using backend-specific GUIs.
 
Consul has a built-in web UI, so we recommend using that if a GUI is needed.
It can be accessed using the `/ui` URL path, for example `http://myconsulserver:8500/ui`
For more information: 
https://www.consul.io/intro/getting-started/ui.html

## Doxygen
`make doc` will generate the doxygen documentation.
To access the resulting documentation, open doc/html/index.html in your
build directory. To install the documentation when calling `make install`
turn on the variable `DOC_INSTALL`.
