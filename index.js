const bindings = require('./build/Release/bindings');

const obj = new bindings.TopicPartition("Test", 1);
obj.setOffset(10);
console.log(obj.err().topic());