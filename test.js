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
    const consumer = new kafka.KafkaConsumer({
        "default_topic_conf": {
            "auto.offset.reset": "smallest"
        },
        "group.id": "asdasdccaasdcasdcasdcasdccsdc",
        "metadata.broker.list": "127.0.0.1:9092",
        "session.timeout.ms": "10000",
        "enable.auto.commit": "false",
    });
    consumer.subscribe(['test_dc.resource_change5']);
    return consumer.consume().then((message) => {
        const stuff = message.payload.toString();
        console.log(stuff);
        consumer.close();
        console.log("CLOSE CONSUMER");
        global.gc();
        console.log("AFTER GC");
    })
    //.then(get);
}

get();

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