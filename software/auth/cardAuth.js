var config = require("../config")
var request = require("request")

module.exports = function(fingerPrint, callback){
	request.post({
		url: config.accounts9 + "/interface/getUserByIdentity",
		json: {
			interfaceSecret: config.interfaceSecret,
			identity: fingerPrint
		},
		strictSSL: true,
	}, function(error, response, body){
		if(error){
			return callback(error);
		}
		if(body.error === 'no-such-user'){
			return callback(null, null);
		}else if(body.error){
			return callback(body.error, null);
		}else{
			if(body.user.groups.some(function(g){
				return config.acceptGroup.some(function(h){
					return h === g;
				});
			})){
				return callback(null, body.user.name);
			}else{
				return callback(null, null);
			}
		}
	});
}