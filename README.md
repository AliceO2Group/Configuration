# Configuration module


## Introduction
The Configuration module provides a way to access Configuration data from various backends in a uniform way. 
Users get an interface to backends by providing a URI to the ConfigurationFactory class, which returns an implementation 
of ConfigurationInterface.
Values can be retrieved from the interface one-by-one using simple getters, or multiple at a time using getRecursive().
The getRecursive() function returns a tree-like data structure, which models the hierarchy of directories or paths.


## Examples
There are usage examples in the file test/Examples.cxx. 
The unit tests may also be useful as examples.


## Dependencies
The configuration module has some external dependencies, this section provides instructions for installing them.

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
make
sudo make install
~~~

### etcdcpp
NOTE: There are two ETCD C++ interface libraries listed on the ETCD github (https://github.com/coreos/etcd/blob/master/Documentation/libraries-and-tools.md)

~~~
git clone https://github.com/suryanathan/etcdcpp
cd etcdcpp
sudo mkdir /usr/local/include/etcdcpp
sudo cp etcd.hpp rapid_reply.hpp /usr/local/include/etcdcpp
~~~

### UriParser
No need to anything currently, it is included in the project.

### libcurl
Should be available in your OS's package manager, or else: https://curl.haxx.se/download.html

### etcd
Note: this is not a compile-time dependency, just an optional runtime-dependency
Follow instructions on: https://github.com/coreos/etcd/releases/
Or use Docker: 
docker run --name=etcd --net=host quay.io/coreos/etcd:v3.0.14


## GUI for etcd
There is a simple Node.js-based GUI available. You may find it useful for simple editing and visualisation of the data.

~~~
yum install nodejs libuv
wget https://github.com/henszey/etcd-browser/archive/master.zip
unzip master.zip
cd etcd-browser-master
ETCD_HOST=127.0.0.1 node server.js
~~~
(For more information: https://github.com/henszey/etcd-browser)


