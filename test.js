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
        global.gc();
    })
    .catch(console.log.bind(console))
    .then(get);
}

for (let i = 0; i < 50; i++)
{
    get();
}

const producer1 = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
const produce1 = () => {
    producer1.produce('test_dc.resource_change5', 'TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11TEST_MESSAGE11')
        .then((offset) => {
            console.log(offset);
            global.gc();
        })
        .then(produce1);
};
produce1();