const kafka = require('./index.js');

// TODO: make it stringify all properties in JS code for simplicity of the API
const consumer = new kafka.KafkaConsumer({
    "default_topic_conf": {
        "auto.offset.reset": "largest"
    },
    "group.id": "asdasdccasdcaasdcasdcaasdcsdasdcccsdc",
    "metadata.broker.list": "127.0.0.1:9092",
    "session.timeout.ms": "10000"
    //"debug": "destroy"
});
consumer.subscribe(['test_dc.resource_change5']);

let time;
let num = 0;
function get() {
    return consumer.consume().then((message) => {
        const stuff = message.payload.toString();

        console.log(stuff, message.offset);
        const offset = new kafka.TopicPartition(message.topicName, message.partition);
        offset.offset = message.offset;
        consumer.commit([offset]);
    })
    .catch(console.log.bind(console));
}

for (let i = 0; i < 100; i++)
{
    get();
}

setTimeout(() => consumer.close(), 5000);

/*const producer1 = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
const produce1 = () => {
    producer1.produce('test_dc.resource_change5', 'TEST_MESSAGE11')
        .then((offset) => {
            console.log(offset);
        })
        .then(() => {
            producer1.close()
        });
};
produce1();

const producer2 = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
const produce2 = () => {
    producer2.produce('test_dc.resource_change5', 'TEST1')
    .then((offset) => {
        console.log(offset);
    })
    .then(() => producer2.close());
};
produce2();*/