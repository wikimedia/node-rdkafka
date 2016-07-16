const kafka = require('./index.js');

// TODO: make it stringify all properties in JS code for simplicity of the API
const consumer = new kafka.KafkaConsumer({
    "default_topic_conf": {
        "auto.offset.reset": "largest"
    },
    "group.id": "asdasdccaasdcasdccsdc",
    "metadata.broker.list": "127.0.0.1:9092",
    "session.timeout.ms": "10000",
    "enable.auto.commit": "false",
    //"debug": "destroy"
});
consumer.subscribe(['test_dc.resource_change5']);

let time;
let num = 0;
function get() {
    return consumer.consume().then((message) => {
        const stuff = message.payload.toString();
        console.log(stuff);
        if (stuff !== 'close') {
            return get();
        }
        /*time = time || new Date().getTime();
        num++;
        if (num % 100 === 0) {
            console.log(num * 1000 / (new Date().getTime() - time))
        }*/
    })
    .catch((e) => console.log(e));
}
get()
.then(() => {
    consumer.close();
})


/*
const producer1 = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
const produce1 = () => {
    producer1.produce('test_dc.resource_change5', 'TEST_MESSAGE11')
        .then((offset) => {
            console.log(offset);
        })
        .then(produce1);
};
produce1();

const producer2 = new kafka.Producer({
    "metadata.broker.list": "127.0.0.1:9092"
});
const produce2 = () => {
    producer2.produce('test_dc.resource_change5', 'TEST_MESSAGE12')
    .then((offset) => {
        console.log(offset);
    })
    .then(produce2);
};
produce2();
*/