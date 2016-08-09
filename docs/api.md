## Classes

<dl>
<dt><a href="#KafkaConsumer">KafkaConsumer</a></dt>
<dd><p>A wrapper of librdkafka high-level <a href="http://docs.confluent.io/2.0.0/clients/librdkafka/classRdKafka_1_1KafkaConsumer.html">KafkaConsumer class</a>
The API is almost a direct mapping for a native API, but most of the
functions are async and promisified.</p>
</dd>
<dt><a href="#Producer">Producer</a></dt>
<dd><p>A wrapper over the librdkafka <a href="http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1Producer.html">Producer</a>
class.</p>
</dd>
<dt><a href="#TopicPartition">TopicPartition</a></dt>
<dd><p>A generic type to hold a single partition and various information about it.
The JS object internally holds a reference to the librdkafka TopicPartition object, so
all property access is mapped to the calls of appropriate C++ methods.</p>
</dd>
</dl>

## Typedefs

<dl>
<dt><a href="#Message">Message</a> : <code>Object</code></dt>
<dd><p>Message returned by the KafkaConsumer. The JS object wraps the native C++ object
returned by librdkafka, so every property read is actually transferred to a C++ object.</p>
<p>It&#39;s mostly important when reading the payload, since the actual payload Buffer is copied
every time the payload is requested.</p>
</dd>
</dl>

<a name="KafkaConsumer"></a>

## KafkaConsumer
A wrapper of librdkafka high-level [KafkaConsumer class](http://docs.confluent.io/2.0.0/clients/librdkafka/classRdKafka_1_1KafkaConsumer.html)
The API is almost a direct mapping for a native API, but most of the
functions are async and promisified.

**Kind**: global class  

* [KafkaConsumer](#KafkaConsumer)
    * [new KafkaConsumer(conf)](#new_KafkaConsumer_new)
    * [.subscribe(topics)](#KafkaConsumer+subscribe)
    * [.consume()](#KafkaConsumer+consume) ⇒ <code>[P.&lt;Message&gt;](#Message)</code>
    * [.commit(commitValues)](#KafkaConsumer+commit)
    * [.close()](#KafkaConsumer+close)

<a name="new_KafkaConsumer_new"></a>

### new KafkaConsumer(conf)
Constructs a new instance of the consumer.


| Param | Type | Description |
| --- | --- | --- |
| conf | <code>Object</code> | An object containing the desired config for this consumer.                      The list of supported properties can be found in                      [librdkafka configuration docs](https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md) |

<a name="KafkaConsumer+subscribe"></a>

### kafkaConsumer.subscribe(topics)
Update the subscription set to topics. Any previous subscription
will be unassigned and unsubscribed first.

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  
**See**: [RdKafka::KafkaConsumer::subscribe](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1KafkaConsumer.html#a7404297cecc9be656026c6c6154ce2bd)  

| Param | Type | Description |
| --- | --- | --- |
| topics | <code>Array.&lt;string&gt;</code> | The list of topics to subscribe too |

<a name="KafkaConsumer+consume"></a>

### kafkaConsumer.consume() ⇒ <code>[P.&lt;Message&gt;](#Message)</code>
Consumes a single message from the queue.

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  
**Returns**: <code>[P.&lt;Message&gt;](#Message)</code> - a P that resolves to a next message in the queue  
**See**: [RdKafka::KafkaConsumer::consume](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1KafkaConsumer.html#a7dc106f1c3b99767a0930a9cf8cabf84)  
<a name="KafkaConsumer+commit"></a>

### kafkaConsumer.commit(commitValues)
Commits the offest for a specific topic+partition

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  
**See**: [RdKafka::KafkaConsumer::commit](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1KafkaConsumer.html#a66a2c7639521e0c9eb25c3417921e318)  

| Param | Type | Description |
| --- | --- | --- |
| commitValues | <code>[Array.&lt;TopicPartition&gt;](#TopicPartition)</code> | An array of TopicPartition objects                           holding topic+partition+offset combinations |

<a name="KafkaConsumer+close"></a>

### kafkaConsumer.close()
Close and shut down the consumer.

This call will block until the following operations are finished:
 - Trigger a local rebalance to void the current assignment
 - Stop consumption for current assignment
 - Commit offsets
 - Leave group

The maximum blocking time is roughly limited to session.timeout.ms.

Client application is responsible for calling this method on shutdown.

NOTE: The close method is synchronous and it's blocking the EventLoop.

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  
**See**: [RdKafka::KafkaConsumer::close](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1KafkaConsumer.html#a5c78a721aa91f3be9903f09ddf084644)  
<a name="Producer"></a>

## Producer
A wrapper over the librdkafka [Producer](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1Producer.html)
class.

**Kind**: global class  

* [Producer](#Producer)
    * [new Producer(conf)](#new_Producer_new)
    * [.produce(topic, partition, payload, [key])](#Producer+produce)
    * [.close()](#Producer+close)

<a name="new_Producer_new"></a>

### new Producer(conf)
Constructs a new producer.


| Param | Type | Description |
| --- | --- | --- |
| conf | <code>Object</code> | An object containing the desired config for this producer.                      The list of supported properties can be found in                      [librdkafka configuration docs](https://github.com/edenhill/librdkafka/blob/master/CONFIGURATION.md) |

<a name="Producer+produce"></a>

### producer.produce(topic, partition, payload, [key])
Send a message to the queue.

**Kind**: instance method of <code>[Producer](#Producer)</code>  
**See**: [RdKafka::Producer::produce](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1Producer.html#ab90a30c5e5fb006a3b4004dc4c9a7923)  

| Param | Type | Description |
| --- | --- | --- |
| topic | <code>string</code> | a name of the topic to send the message to |
| partition | <code>Number</code> | number of a partition to send the message to |
| payload | <code>string</code> | the contents of the message |
| [key] | <code>string</code> | an optional message key, if defined it will be                       passed to the topic partitioner as well as be sent                       with the message to the broker and passed on to the consumer. |

<a name="Producer+close"></a>

### producer.close()
Close and shut down the producer.

 Internally the producer runs a background thread for polling it
 so that message delivery reports could be received in a timely fashion.
 This method stops background thread and clear up the resources.

**Kind**: instance method of <code>[Producer](#Producer)</code>  
<a name="TopicPartition"></a>

## TopicPartition
A generic type to hold a single partition and various information about it.
The JS object internally holds a reference to the librdkafka TopicPartition object, so
all property access is mapped to the calls of appropriate C++ methods.

**Kind**: global class  
**See**: [RdKafka::TopicPartition](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1TopicPartition.html)  
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| topicName | <code>string</code> | The topic, readonly |
| partition | <code>number</code> | The partition, readonly |
| offset | <code>number</code> | The offset |
| err | <code>ErrorCode</code> | The error code, readonly |

<a name="new_TopicPartition_new"></a>

### new module.exports.TopicPartition(topic, partition)
Create topic+partition object for topic and partition,
analog of librdkafka RdKafka::TopicPartition::create.


| Param | Type | Description |
| --- | --- | --- |
| topic | <code>string</code> | The topic |
| partition | <code>number</code> | The partition |

<a name="Message"></a>

## Message : <code>Object</code>
Message returned by the KafkaConsumer. The JS object wraps the native C++ object
returned by librdkafka, so every property read is actually transferred to a C++ object.

It's mostly important when reading the payload, since the actual payload Buffer is copied
every time the payload is requested.

**Kind**: global typedef  
**Access:** public  
**See**: [RdKafka::Message](http://docs.confluent.io/3.0.0/clients/librdkafka/classRdKafka_1_1Message.html)  
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| err | <code>int</code> | the error code if there was an error |
| errStr | <code>string</code> | the string describing the error |
| topicName | <code>string</code> | the name of the topic this message belonged to |
| partition | <code>Number</code> | the number of a partition which this message belonged to |
| payload | <code>buffer</code> | the payload of the message |
| key | <code>string</code> &#124; <code>undefined</code> | the key of the message if it's defined |
| timestamp | <code>Object</code> &#124; <code>undefined</code> | the message timestamp if it's available |

