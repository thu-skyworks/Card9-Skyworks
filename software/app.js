var net = require('net');
var config = require('./config');
var logger = require('./logger');
var parser = require('./utils/ProtocolParser');
var defines = require('./utils/defines');
var cardAuth = require('./auth/cardAuth');
var encoder = require('./utils/ProtocolBinarify');
var server = net.createServer(function(c) { //'connection' listener
  var client = c.remoteAddress + ':' + c.remotePort;
  logger({
    event: 'Client connected', 
    client: client,
    reason: 'System',
  });
  c.on('end', function() {
    logger({
			event: 'Client disconnected',
			client: client,
			reason: 'System',
    });
  }).on('error', function(){
  	c.end();
  });
  
  c.pipe((new parser).on('packet', function(p){
  	switch(p.type){
  		case defines.event:
  			logger({
  				event: defines.events.map[p.data.type],
  				client: client,
  				reason: 'Client',
  			});
  			break;
  		case defines.request:
  			switch(p.data.type){
  				case defines.requests.auth:
  					logger({
  						event: 'Authenticating Card',
  						client: client,
  						reason: 'Client',
  						data: p.data.type.data,
  					});
						cardAuth(p.data.type.data, function(err, pass){
							if(err){
								logger({
									event: 'Error',
									client: client,
									reason: 'System',
									err: err,
								});
								c.write(encoder.response(defines.responses.natural));
							}else{
								if(pass !== null){
									logger({
										event: 'Auth Pass',
										reason: pass,
									});
									c.write(encoder.response(defines.responses.positive));
								}else{
									logger({
										event: 'Auth Deny',
										reason: 'System',
										identity: p.data.type.data,
									});
									c.write(encoder.response(defines.responses.negative));
								}
							}
						});
  					break;
  				default:
  					//What the hell?
  					break;
  			}
  			break;
  		default:
  			//What the hell?
  			break;
  	}
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