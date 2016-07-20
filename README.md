# node-rdkafka
[![Version](https://img.shields.io/npm/v/rdkafka.svg?maxAge=2592000&style=flat-square)](https://www.npmjs.com/package/rdkafka)
[![Travis](https://img.shields.io/travis/wikimedia/node-rdkafka.svg?maxAge=2592000&style=flat-square)](https://travis-ci.org/wikimedia/node-rdkafka)
[![Dependencies](https://img.shields.io/david/wikimedia/node-rdkafka.svg?maxAge=2592000&style=flat-square)](https://david-dm.org/wikimedia/node-rdkafka)
[![License](https://img.shields.io/github/license/wikimedia/node-rdkafka.svg?maxAge=2592000&style=flat-square)](https://github.com/wikimedia/node-rdkafka/blob/master/LICENSE)


## Building with node-gyp

`binding.gyp` supports buidling with the local copy of librdkafka in
deps/librdkafka, or on Linux with a globally installed librdkafka binaries and
headers.

Building with the local copy of librdkafka is the default.  Just do

```
git submodule update --init
node-gyp configure
node-gyp build
````

To build against a globally installed librdkafka (e.g. one installed
with .deb packages), do

```
node-gyp configure --BUILD_LIBRDKAKFA=0
node-gyp build
```
