var fs = require('fs');
var config = require('./config');
var logFileReady = false;
var logStream = fs.createWriteStream(config.logFile, {
	flags: 'a',
	encoding: null,
}).on('open', function(){
	logFileReady = true;
	toWriteBeforeOpen.forEach(function(e){
		writeLog(e);
	});
	toWriteBeforeOpen = [];
});

var toWriteBeforeOpen = [];

var writeLog = module.exports = function(jsonObj){
	if(logFileReady){
		logStream.write(JSON.stringify(jsonObj));
		logStream.write(", \n");
	}else{
		toWriteBeforeOpen.push(jsonObj);
	}
};

