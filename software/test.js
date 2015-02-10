var decoder = require('./utils/ProtocolParser');

var d = new decoder;

d.on('packet', function(p){
	console.log(p);
})

d.write(new Buffer([0xA1, 0x01, 0x02]));
d.write(new Buffer([0x00, 0x01, 0x02]));
