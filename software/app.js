var net = require('net');
var config = require('./config');
var logger = require('./logger');
var parser = require('./utils/ProtocolParser');
var server = net.createServer(function(c) { //'connection' listener
  logger({
    event: 'Client connected', 
    client: c.remoteAddress + ':' + c.remotePort,
    reason: 'System',
  });
  c.on('end', function() {
    logger({
			event: 'Client disconnected',
			client: c.remoteAddress + ':' + c.remotePort,
			reason: 'System',
    });
  }).on('error', function(){
  	c.end();
  });
  c.pipe((new parser).on('packet', function(p){
  	console.log(p);
  }));
  
});
server.listen(config.bindPort, function(){ //'listening' listener
  logger("Card9 server started");
}).on('error', function(e){
	if(e.code == 'EADDRINUSE'){
		logger("Error: Address already in use, exiting...", function(){
			process.exit(1);
		});      
	}
});