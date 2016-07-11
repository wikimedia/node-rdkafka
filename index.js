const bindings = require('./build/Release/bindings');

const consumer = new bindings.KafkaConsumer();
consumer.subscribe( [ 'test_dc.resource_change' ]);
while (true) {
    const message = consumer.consume(1000);
    if (message.err() === 0) {
        console.log('Got message: ' + message.payload().toString());
    } else {
        console.log(message.errStr());
    }
}
