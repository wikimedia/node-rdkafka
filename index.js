const bindings = require('./build/Debug/bindings');

const consumer = new bindings.KafkaConsumer();
consumer.subscribe( [ 'test_dc.resource_change' ]);

function get() {
    consumer.consume(function (result) {
        console.log(result.errStr());
        global.gc();
    });
    //setTimeout(get, 1000);
}
get();
