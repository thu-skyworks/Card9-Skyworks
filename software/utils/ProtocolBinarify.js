module.exports = {
	command: function(type){
		return new Buffer([0xA1, 0x01, 0x01, 0x00, type]);
	}
	response: function(type){
		return new Buffer([0xA1, 0x03, 0x01, 0x00, type]);
	}
};