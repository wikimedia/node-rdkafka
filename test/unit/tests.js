"use strict";

const assert = require('assert');
const P = require('bluebird');

const kafka  = require('../../index');

describe('Produce/Consume', function() {
    this.timeout(30000);

    const producer = new kafka.Producer({
        'metadata.broker.list': 'localhost:9092'
    });
    const consumer = new kafka.KafkaConsumer({
        'metadata.broker.list': 'localhost:9092',
        'group.id': 'test_group_id',
        'fetch.wait.max.ms': 1,
        'fetch.min.bytes': 1,
        'queue.buffering.max.ms': 1,
        'default_topic_conf': {
            'auto.offset.reset': 'smallest'
        }
    });

    it('Should consume a message is just produced', () => {
        let offset;
        return producer.produce('test_topic_produce_consume', 0, 'Testing')
        .then((newOffset) => {
            offset = newOffset;
        })
        .then(() => {
            consumer.subscribe([ 'test_topic_produce_consume' ]);
            return consumer.consume();
        })
        .then((message) => {
            assert.deepEqual(message.topicName, 'test_topic_produce_consume', 'Topic name should match');
            assert.deepEqual(message.partition, 0, 'Partition should match');
            assert.deepEqual(message.offset, offset, 'Offset should match');
            assert.deepEqual(message.payload.toString(), 'Testing', 'Payload should match');
        });
    });
});
