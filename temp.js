"use strict";

const kafka = require('./index');

const producer = new kafka.Producer({
 'metadata.broker.list': 'localhost:9092'
 });
const consumer1 = new kafka.KafkaConsumer({
    'metadata.broker.list': 'localhost:9092',
    'group.id': 'my_group_id',
    'fetch.wait.max.ms': '1',
    'fetch.min.bytes': '1',
    'queue.buffering.max.ms': '1'
});
consumer1.subscribe(['test_dc.resource_change']);
consumer1.on('error', (err) => {
    console.log('CONS1 ERR: ', err);
});
consumer1.on('log', (err) => {
    console.log('CONS1 LOG: ', err);
});
consumer1.on('stats', (err) => {
    console.log('CONS1 STATS: ', err);
});
/*let startTime;
let i = 0;
function get1() {
    return consumer1.consume()
    .then((message) => {
        startTime = startTime || new Date();
        if ((i++) % 10000 === 0) {
            console.log(i * 1000 / (new Date() - startTime));
        }
    })
    .then(get1);
}
for (let i = 0; i < 1000; i++) {
    get1();
}*/

const consumer2 = new kafka.KafkaConsumer({
    'metadata.broker.list': 'localhost:9092',
    'group.id': 'my_group_id',
    'fetch.wait.max.ms': '1',
    'fetch.min.bytes': '1',
    'queue.buffering.max.ms': '1',
});
consumer2.subscribe(['example_topic']);
consumer2.on('error', (err) => {
    console.log('CONS2 ERR: ', err);
});
consumer2.on('log', (err) => {
    console.log('CONS2 LOG: ', err);
});

function get2() {
    producer.produce('example_topic', 0, 'Test message', 'TEST KEY')
    .then((offset) => {
        console.log(`Produced a message with offset ${offset}`);

        return consumer2.consume()
        .then((message) => {
            console.log(`Got a message: \n` +
                `   topic: ${message.topicName}\n` +
                `   partition: ${message.partition}\n` +
                `   offset: ${message.offset}\n` +
                `   payload: ${message.payload.toString()}\n`);
        })
        .then(get2);
    });
}
get2();