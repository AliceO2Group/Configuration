#!/bin/bash
#
# Example file for compiling the etcd .proto files
#
# Prerequisites:
# install protobuf (minimum version 3.0.0)
# install grpc
# install googleapis

SRC_DIR="/home/pboescho/hackathon/Configuration/src/EtcdV3/proto"
OUT_DIR="/home/pboescho/hackathon/Configuration/src/EtcdV3/proto"
GRPC_DIR="/home/pboescho/grpc"
GAPI_DIR="/home/pboescho/googleapis"

CMD="/usr/local/bin/protoc --cpp_out=$OUT_DIR --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) -I $SRC_DIR --grpc_out=$OUT_DIR"

echo "Building kv.proto"
$CMD $SRC_DIR/kv.proto 

echo "Building rpc.proto"
$CMD -I $GAPI_DIR $SRC_DIR/rpc.proto 

echo "Building auth.proto"
$CMD $SRC_DIR/auth.proto 

