const bindings = require('./build/Debug/bindings');

const consumer = new bindings.KafkaConsumer();
consumer.subscribe( [ 'test_dc.resource_change' ]);

function get() {
    consumer.consume(function (error, result) {
        if (error) {
            console.log(error + ' ' + error.code);
        } else {
            console.log(result.timestamp);
        }
        get();
    });
    //setTimeout(get, 1000);
}
get();
