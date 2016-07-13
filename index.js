const bindings = require('./build/Release/bindings');

const consumer = new bindings.KafkaConsumer({
    "default_topic_conf": {
        "auto.offset.reset": "smallest",
    },
    "group.id": "test_test_test_test5",
    "metadata.broker.list": "127.0.0.1:9092",
});
const producer = new bindings.Producer();

consumer.subscribe( [ 'test_dc.resource_change' ]);
var num = 0;
var time;

function get() {
    consumer.consume(function (error, result) {
        time = time || new Date().getTime();
        if (error) {
            console.log(error + ' ' + error.code);
        } else {
            num++;
            if (num % 100 === 0) {
                console.log(num / ((new Date() - time) / 1000));
            }
        }
        get();
    });
}
get();
get();
get();
get();
get();
get();
get();
get();
get();
get();
get();
get();
