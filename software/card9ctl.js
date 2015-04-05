"use continuation"


var defines = require('./utils/defines');
var config = require('./config');
var net = require('net');

globalEvent = new (require('events').EventEmitter);

function usage(){
	process.stderr.write(
'一个控制 Card9 的工具                        \n\
用法: card9ctl <command>                 \n\
                                         \n\
<command> 可以是：                        \n\
    doorOpen           打开门禁            \n\
    alarmOff           消除警报音          \n\
    addCard <userName> 为<userName> 添加卡片\n\
', 'utf-8');
}

var command = -1;
switch(process.argv[2]){
	case 'doorOpen':
		command = defines.commands.doDoorOpen;
		break;
	case 'alarmOff':
		command = defines.commands.doAlarmOff;
		break;
	case 'addCard':
	  if(process.argv[3]){
	    command = -2;
	    break;
	  }
	default:
		usage();
		process.stderr.once('drain', function(){
			process.exit(1);
		});
		break;
}

var createSession = function(callback){
  var client = net.createConnection({path: config.sockFile}, callback).once('error', function(err){
		console.error("在通过“" + config.sockFile + "”与守护进程建立连接时，发生了错误：");
		console.error(err);
		console.error("可能权限不足或守护进程未启动。");
		process.exit(1);
	});
	return client;
}
if(command == -2){
  var username = process.argv[3];
  var lenBuffer = new Buffer(4);
  var readLenth = 0;
  var identity, identityLength;
  var client = createSession(function(){
    try{
      var User = require('./models/user');
      var user;
      User.getOrCreateByName(process.argv[3], obtain(user));
      console.log("等待刷卡...");
      globalEvent.on('card', cont(identity));
      user.addIdentity(identity, obtain());
      console.log("成功");
      process.exit(0);
    }catch(err){
      console.error("错误：" + err);
      process.exit(0);
    }
  }).on('data', function(chunk){
    for(var i=0; i < chunk.length; i++){
      if(readLenth < 4){
        lenBuffer[readLenth++] = chunk[i];
        if(readLenth == 4){
          identityLength = lenBuffer.readUInt32LE(0);
          identity = new Buffer(identityLength);
        }
      }else{
        identity[ 3 - (--identityLength)] = chunk[i];
        if(identityLength == 0){
          globalEvent.emit('card', identity);
          readLenth = 0;
        }
      }
    }
  });
}else if(command != -1){
	var client = createSession(function(){
		client.write(new Buffer([command]));
		client.once('drain', function(){
			client.end();
			console.log("成功发送命令。");
		});
	});
}