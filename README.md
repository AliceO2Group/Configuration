# Configuration module


## Dependencies
The configuration module has some external dependencies, this section provides instructions for installing them.

### etcd
Follow instructions on: https://github.com/coreos/etcd/releases/

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
git clone -b $(curl -L http://grpc.io/release) https://github.com/grpc/grpc
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


## How to get and use the etcd Configuration back-end
A stand-alone installation procedure, for simple prototyping usage:

~~~
wget https://github.com/coreos/etcd/releases/download/v2.2.5/etcd-v2.2.5-linux-amd64.tar.gz
tar xzvf etcd-v2.2.5-linux-amd64.tar.gz
cd etcd-v2.2.5-linux-amd64
./etcd
~~~
(For more information: https://github.com/coreos/etcd/releases/tag/v2.2.5)

This starts an etcd instance with default parameters. It will store its database in the current directory. To use it, pass the URI "etcd://127.0.0.1:4001" to the Configuration API. For example:

~~~
auto conf = ConfigurationFactory::getConfiguration("etcd://127.0.0.1:4001");
conf->put<int>("/my_directory/my_value", 123);
int x = conf->get<int>("/my_directory/my_value");
~~~

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


