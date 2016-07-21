const kafka = require('./index');

const producer = new kafka.Producer({
    'metadata.broker.list': 'localhost:9092'
});

producer.produce('example_topic', 'Test message')
.then((offset) => {
    console.log(`Produced a message with offset ${offset}`);

    const consumer = new kafka.KafkaConsumer({
        'metadata.broker.list': 'localhost:9092',
        'group.id': 'my_group_id',
        'fetch.wait.max.ms': '1',
        'fetch.min.bytes': '1',
        'queue.buffering.max.ms': '1',
    });
    consumer.subscribe([ 'example_topic' ]);
    return consumer.consume()
    .then((message) => {
        console.log(`Got a message: \n` +
            `   topic: ${message.topicName}\n` +
            `   partition: ${message.partition}\n` +
            `   offset: ${message.offset}\n` +
            `   payload: ${message.payload.toString()}\n`);
    })
    .then(() => {
        producer.close();
        consumer.close();
    });
});