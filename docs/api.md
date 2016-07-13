## Classes

<dl>
<dt><a href="#KafkaConsumer">KafkaConsumer</a></dt>
<dd></dd>
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
**Kind**: global class  

* [KafkaConsumer](#KafkaConsumer)
    * [.consume()](#KafkaConsumer+consume) ⇒ <code>[Promise.&lt;Message&gt;](#Message)</code>
    * [.commit(commitValues)](#KafkaConsumer+commit)

<a name="KafkaConsumer+consume"></a>

### kafkaConsumer.consume() ⇒ <code>[Promise.&lt;Message&gt;](#Message)</code>
Consumes a single message from the queue.

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  
**Returns**: <code>[Promise.&lt;Message&gt;](#Message)</code> - a promise that resolves to a next message in the queue  
<a name="KafkaConsumer+commit"></a>

### kafkaConsumer.commit(commitValues)
Commits the offest for a specific topic+partition

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  

| Param | Type | Description |
| --- | --- | --- |
| commitValues | <code>[Array.&lt;TopicPartition&gt;](#TopicPartition)</code> &#124; <code>[Message](#Message)</code> | two options are supported:      - An array of TopicPartition objects holding topic+partition+offset combinations      - A Message object. That must be the message provided by the consume method. |

<a name="TopicPartition"></a>

## TopicPartition
A generic type to hold a single partition and various information about it.
The JS object internally holds a reference to the librdkafka TopicPartition object, so
all property access is mapped to the calls of appropriate C++ methods.

**Kind**: global class  
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| topic | <code>string</code> | The topic, readonly |
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
**Properties**

| Name | Type | Description |
| --- | --- | --- |
| err | <code>int</code> | the error code if there was an error |
| errStr | <code>string</code> | the string describing the error |
| topicName | <code>string</code> | the name of the topic this message belonged to |
| partition | <code>Number</code> | the number of a partition which this message belonged to |
| payload | <code>Buffer</code> | the payload of the message |
| key | <code>string</code> &#124; <code>undefined</code> | the key of the message if it's defined |
| timestamp | <code>Object</code> &#124; <code>undefined</code> | the message timestamp if it's available |

