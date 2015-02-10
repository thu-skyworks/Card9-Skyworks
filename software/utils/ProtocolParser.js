var Writable = require('stream').Writable;
var util = require('util');

util.inherits(ProtocolParser, Writable);

function ProtocolParser(options){
	if(options === undefined){
		options = {};
	}
	options.decodeStrings = true;
	options.objectMode = false;
	if (!(this instanceof ProtocolParser)){
    return new ProtocolParser(options);
  }
  
  Writable.call(this, options);
	this._clearResult();
  
}

ProtocolParser.prototype._write = function(chunk, encoding, callback){
	for(var i=0; i < chunk.length; i++){
		switch(this._state){
			case 0://开始数据读取
				this._result.version = chunk.readUInt8(i) & 0x0f;
				this._state = 1;
				break;
			case 1://读取数据类型
				this._result.type = chunk.readUInt8(i);
				this._state = 2;
				break;
			case 2://读取大小
				 this._buf[this._pos++] = chunk[i];
				 if(this._pos == 2){
				   this._bytesLeft = this._buf.readUInt16LE(0);
				   this._state = 3;
				   this._pos = 0;
				   this._buf = new Buffer(this._bytesLeft - 1);
				 }
				 break;
			case 3://读取下一个参数
				if(this._bytesLeft > 0){
					this._result.data.type = chunk.readUInt8(i);
					this._bytesLeft--;
					this._state = 4;
				}
				break;
			case 4://读取剩余的数据
			  if(this._bytesLeft > 0){
			    this._buf[this._pos++] = chunk[i];
			  	this._bytesLeft--;
			  }
			  break;
		}
		if(this._bytesLeft === 0 && (this._state === 3 || this._state === 4)){
			this._result.data.param = this._buf;
			this.emit('packet', this._result);
			this._clearResult();
		}
	}
	callback(null);
}

ProtocolParser.prototype._clearResult = function(){
	this._result = {
		version: 0,
		type: null,
		data: {
			type: null,
			param: null,
		}
	};
	this._state = 0;
	this._buf = new Buffer(6);
	this._pos = 0;
	this._bytesLeft = 0;
}

module.exports = ProtocolParser;