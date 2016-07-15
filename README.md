# Building with node-gyp

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
