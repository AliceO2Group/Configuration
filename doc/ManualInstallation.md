### Manual installation
This section provides instructions for manually installing the Configuration module and its dependencies.

First make sure you have the `devtoolset-6`
```
sudo yum -y install centos-release-scl
sudo yum-config-manager --enable rhel-server-rhscl-7-rpms
sudo yum -y install devtoolset-6
source /opt/rh/devtoolset-6/enable  # Do this every session, or add to your ~/.bashrc
```

Get remaining additional prerequisite packages:
```
sudo yum -y install wget git cmake cmake3 autoconf automake
```

#### libcurl
Should be available in your OS's package manager, or else: https://curl.haxx.se/download.html
For CC7:
```
sudo yum -y install curl-devel
```

#### Boost
Note: ppconsul requires 1.55 or newer
```
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
```

#### ppconsul
```
cd /tmp
git clone https://github.com/oliora/ppconsul.git
cd ppconsul
git checkout 8ade80d0528b563d4b58bc4f09815fc1e3d5be19
mkdir build; cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make -j
sudo cp output/*.so /usr/local/lib/
sudo cp -r ../include/* /usr/local/include/
```

#### Configuration
```
cd /tmp
git clone https://github.com/AliceO2Group/Configuration.git
cd Configuration; mkdir build; cd build
cmake3 ..
make -j
sudo make install
```
