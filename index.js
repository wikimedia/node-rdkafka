"use strict";

const ErrorCode = require('./lib/ErrorCode');
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
 * @property {buffer} payload the payload of the message
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
                    return reject(error);
                }
                return resolve(value);
            });
        });
    }

    /**
     * Commits the offest for a specific topic+partition
     *
     * @param {TopicPartition[]} commitValues An array of TopicPartition objects
     *                           holding topic+partition+offset combinations
     */
    commit(commitValues) {
        this.impl.commit(commitValues);
    }

    /**
     * Close and shut down the proper.
     *
     * This call will block until the following operations are finished:
     *  - Trigger a local rebalance to void the current assignment
     *  - Stop consumption for current assignment
     *  - Commit offsets
     *  - Leave group
     *
     * The maximum blocking time is roughly limited to session.timeout.ms.
     *
     * Client application is responsible for calling this method on shutdown.
     */
    close() {
        this.impl.close();
    }
}

class Producer {
    constructor(conf) {
        this.impl = new bindings.Producer(conf);
    }


    produce(topic, payload) {
        return new Promise((resolve, reject) => {
            this.impl.produce(topic, payload, (error, offset) => {
                if (error) {
                    return reject(error);
                }
                return resolve(offset);
            });
        });
    }

    close() {
        return this.impl.close();
    }
}

module.exports.KafkaConsumer = KafkaConsumer;
module.exports.Producer = Producer;
/**
 * @classdesc A generic type to hold a single partition and various information about it.
 * The JS object internally holds a reference to the librdkafka TopicPartition object, so
 * all property access is mapped to the calls of appropriate C++ methods.
 *
 * @class
 * @description Create topic+partition object for topic and partition,
 * analog of librdkafka RdKafka::TopicPartition::create.
 * @param {string} topic The topic
 * @param {number} partition The partition
 *
 * @property {string} topic The topic, readonly
 * @property {number} partition The partition, readonly
 * @property {number} offset The offset
 * @property {ErrorCode} err The error code, readonly
 */
module.exports.TopicPartition = bindings.TopicPartition;
module.exports.ErrorCode = ErrorCode;