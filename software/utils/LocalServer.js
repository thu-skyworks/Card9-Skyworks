var net = require('net');
var defines = require('./defines');
var fs = require('fs');
var globalEvent = require('../globalEvent');
var server = net.createServer(function(c){
	c.on('data', function(chunk){
		server.emit('command', chunk[0]);
		c.end();
	});
	c.on('end', function(){
	  globalEvent.removeListener('cardScan', listenToCardScan);
	});
	c.on('error', function(){
	  globalEvent.removeListener('cardScan', listenToCardScan);
	})
	
	var listenToCardScan = function(ident){
	  var length = new Buffer(4);
	  length.writeUInt32LE(ident.length, 0);
	  c.write(length);
	  c.write(ident);
	}
	
	globalEvent.on('cardScan', listenToCardScan);
	
});
server.create = function(sockFile, callback){
	fs.unlink(sockFile, function(){
		server.listen(sockFile, callback).once('listen', function(){
		  fs.chmodSync(sockFile, 0755);
		});
	});
};

module.exports = server;