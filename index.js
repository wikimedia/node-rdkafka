const Promise = require('bluebird');
const bindings = require('./build/Release/bindings');

/**
 * Message returned by the KafkaConsumer. The JS object wraps the native C++ object
 * returned by librdkafka, so every property read is actually transferred to a C++ object.
 *
 * It's mostly important when reading the payload, since the actual payload Buffer is copied
 * every time the payload is requested.
 *
 * @typedef Message
 * @public
 * @type Object
 * @property {int} err the error code if there was an error
 * @property {string} errStr the string describing the error
 * @property {string} topicName the name of the topic this message belonged to
 * @property {Number} partition the number of a partition which this message belonged to
 * @property {Buffer} payload the payload of the message
 * @property {string|undefined} key the key of the message if it's defined
 * @property {Object|undefined} timestamp the message timestamp if it's available
 */

/**
 *
 */
class KafkaConsumer {
    constructor(conf) {
        this.impl = new bindings.KafkaConsumer(conf);
    }

    subscribe(topics) {
        return this.impl.subscribe(topics);
    }

    /**
     * Consumes a single message from the queue.
     *
     * @returns {Promise<Message>} a promise that resolves to a next message in the queue
     */
    consume() {
        return new Promise((resolve, reject) => {
            this.impl.consume((error, value) => {
                if (error) {
                    reject(error);
                }
                resolve(value);
            });
        });
    }
}

class Producer {
    constructor(conf) {
        this.impl = new bindings.Producer(conf);
    }

    produce(topic, payload) {
        this.impl.produce(topic, payload);
    }
}

module.exports.KafkaConsumer = KafkaConsumer;
module.exports.Producer = Producer;