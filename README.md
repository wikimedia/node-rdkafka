## Classes

<dl>
<dt><a href="#KafkaConsumer">KafkaConsumer</a></dt>
<dd></dd>
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
<a name="KafkaConsumer+consume"></a>

### kafkaConsumer.consume() ⇒ <code>[Promise.&lt;Message&gt;](#Message)</code>
Consumes a single message from the queue.

**Kind**: instance method of <code>[KafkaConsumer](#KafkaConsumer)</code>  
**Returns**: <code>[Promise.&lt;Message&gt;](#Message)</code> - a promise that resolves to a next message in the queue  
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

