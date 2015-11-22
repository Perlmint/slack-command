# slacpp
[![Build Status](https://travis-ci.org/Perlmint/slacpp.png)](https://travis-ci.org/Perlmint/slacpp)
Slack api header only framework in c++ - slash command server, RTM api and other api

## Requiements

- CMake(for build examples)
- Boost(asio)
- Openssl(>= 1.0.0)
- C++ compiler(support C++14 standard)

## Build examples

``` bash
mkdir build
cd build
cmkae ..
make
```
### link error about `_ERR_remove_thread_state` on OSX
slacpp needs openssl(>= 1.0.0).  
specify openssl root for cmake `-DOPENSSL_ROOT_DIR=${OPENSSL_ROOT}`
