var logger = require('./logger');
logger("Started");
setTimeout(function(){
	logger("End");
},1000);