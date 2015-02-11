var net = require('net');
var defines = require('./defines');
var fs = require('fs');
var server = net.createServer(function(c){
	c.on('data', function(chunk){
		server.emit('command', chunk[0]);
		c.end();
	});
});
server.create = function(sockFile, callback){
	fs.unlink(sockFile, function(){
		server.listen(sockFile, callback);
	});
};
module.exports = server;