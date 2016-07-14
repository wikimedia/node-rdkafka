const kafka = require('./index.js');

// TODO: make it stringify all properties in JS code for simplicity of the API
const consumer = new kafka.KafkaConsumer({
    "default_topic_conf": {
        "auto.offset.reset": "smallest"
    },
    "group.id": "something_elseasasdcasdcdasdcc",
    "metadata.broker.list": "127.0.0.1:9092"
});
consumer.subscribe(['test_dc.resource_change5']);

let time;
let num = 0;
function get() {
    consumer.consume().then((message) => {
        console.log(message.payload.toString());
        /*time = time || new Date().getTime();
        num++;
        if (num % 100 === 0) {
            console.log(num * 1000 / (new Date().getTime() - time))
        }*/
    })
    .then(get);
}
get();


const producer = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
const produce = () => {
    producer.produce('test_dc.resource_change5', 'TEST_MESSAGE11')
        .then((offset) => {
            console.log(offset);
            global.gc();
        })
        .then(produce);
};
produce();
