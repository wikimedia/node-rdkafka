# node-rdkafka
[![Version](https://img.shields.io/npm/v/rdkafka.svg?maxAge=2592000&style=flat-square)](https://www.npmjs.com/package/rdkafka)
[![Travis](https://img.shields.io/travis/wikimedia/node-rdkafka.svg?maxAge=2592000&style=flat-square)](https://travis-ci.org/wikimedia/node-rdkafka)
[![Dependencies](https://img.shields.io/david/wikimedia/node-rdkafka.svg?maxAge=2592000&style=flat-square)](https://david-dm.org/wikimedia/node-rdkafka)
[![License](https://img.shields.io/github/license/wikimedia/node-rdkafka.svg?maxAge=2592000&style=flat-square)](https://github.com/wikimedia/node-rdkafka/blob/master/LICENSE)

Node.js bindings for the C++ API of the [librdkafka](https://github.com/edenhill/librdkafka) client for [Apache Kafka](http://kafka.apache.org) messaging system. The library binds to the `Producer` and high-level `KafkaConsumer` and tries to mimic the API of `librdkafka`, returning promises where appropriate. This client requires Apache Kafka >0.9 and node.js >4

Example usage:
```javascript
const kafka = require('rdkafka');

const producer = new kafka.Producer({
    'metadata.broker.list': 'localhost:9092'
});

producer.produce('example_topic', 0, 'Test message')
.then(() => {    
    const consumer = new kafka.KafkaConsumer({
        'metadata.broker.list': 'localhost:9092',
        'group.id': 'my_group_id',
        'fetch.wait.max.ms': 1,
        'fetch.min.bytes': 1,
        'queue.buffering.max.ms': 1,
        'enable.auto.commit': false
    });
    consumer.subscribe([ 'example_topic' ]);
    return consumer.consume()
    .then((message) => {
        console.log(`Got a message: \n` +
            `   topic: ${message.topicName}\n` +
            `   partition: ${message.partition}\n` +
            `   offset: ${message.offset}\n` +
            `   payload: ${message.payload.toString()}\n`);
        const commitRequest = new kafka.TopicPartition(message.topic, 
            message.partition, 
            message.offset + 1);
        return consumer.commit([ commitRequest ]);
    })
    .then(() => {
        producer.close();
        consumer.close();
    });
});
```

## API Documentation
  Can be found [here](https://github.com/wikimedia/node-rdkafka/blob/master/docs/api.md)

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
node-gyp --BUILD_LIBRDKAKFA=0 build
```

##
