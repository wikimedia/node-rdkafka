const bindings = require('./build/Debug/bindings');

const consumer = new bindings.KafkaConsumer();
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
