module.exports = {
	bindAddress: "0.0.0.0",
	bindPort   : 57005,
	logFile    : "./access.log",
	sockFile   : "/tmp/card9.sock",
	dbAddr     : "mongodb://localhost/card9",
  setUid     : "", //should not be root; should have access to the source code
	interfaceSecret : "Example",
	acceptGroup: ["access-door"],
};