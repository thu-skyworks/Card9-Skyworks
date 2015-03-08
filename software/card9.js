#!/usr/bin/env node
var net = require('net');
var config = require('./config');
var logger = require('./logger');
var parser = require('./utils/ProtocolParser');
var defines = require('./utils/defines');
var cardAuth = require('./auth/cardAuth');
var encoder = require('./utils/ProtocolBinarify');
var local = require('./utils/LocalServer');
var server = net.createServer(function(c) { //'connection' listener
  var client = c.remoteAddress + ':' + c.remotePort;
  logger({
    event: 'Client connected', 
    client: client,
    reason: 'System',
  });
  var timerId = setInterval(function(){
    c.write(encoder.command(defines.commands.heartBeep));
  }, 3000);
  var cleanAction = function(){
    local.removeListener('command', commandListener);
    c.end();
    clearInterval(timerId);
  }
  c.on('end', cleanAction);
  c.on('error', cleanAction);
  c.on('end', function() {
    logger({
			event: 'Client disconnected',
			client: client,
			reason: 'System',
    });
  }).on('error', function(){
  	logger({
			event: 'Client connection lost',
			client: client,
			reason: 'System',
    });
  });
  var commandListener = function(command){
  	switch(command){
  		case defines.commands.doDoorOpen: 
  		case defines.commands.doAlarmOff:
  			logger({
					event: defines.commands.map[command],
					reason: 'root',
				});
				c.write(encoder.command(command));
				break;
			default:
				//Not Allow
				break;
  	}
  }
  local.on('command', commandListener);
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
  						data: p.data.param,
  					});
						cardAuth(p.data.param, function(err, pass){
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
										identity: p.data.param,
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
server.listen(config.bindPort, config.bindAddress, function(){ //'listening' listener
  logger("Card9 server started");
  local.create(config.sockFile, function(){
  	if(config.setUid){
			process.setuid(config.setUid);
		}
  })
}).on('error', function(e){
	if(e.code == 'EADDRINUSE'){
		logger("Error: Address already in use, exiting...", function(){
			process.exit(1);
		});      
	}
});
module.exports = server;