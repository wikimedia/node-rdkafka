const kafka = require('./index.js');

// TODO: make it stringify all properties in JS code for simplicity of the API
const consumer = new kafka.KafkaConsumer({
    "default_topic_conf": {
        "auto.offset.reset": "smallest"
    },
    "group.id": "test_test_test_test9",
    "metadata.broker.list": "127.0.0.1:9092",
    "enable.auto.commit": "false"
});
consumer.subscribe(['test_dc.resource_change1']);

function get() {
    consumer.consume().then((message) => {
        consumer.commit(message);
        console.log(message.payload.toString());
    })
    .then(get);
}
get();

const producer = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
producer.produce('test_dc.resource_change1', 'TEST_MESSAGE');