//native c code 8 packet per second
var dgram = require("dgram");
var count=0;
var message = new Buffer("Some byte djs  klsfj ls ljf ljf ldf dlfkjg kldfg kd lkdf jgkdf jgkdfjg l");
console.log('Timeout ran at ' + new Date().toTimeString());
var server = dgram.createSocket("udp4");
server.on("error", function (err) {
  console.log("server error:\n" + err.stack);
  server.close();
});

server.on("message", function (msg, rinfo) {
  //console.log("server got: " + msg + " from " +rinfo.address + ":" + rinfo.port);
	count=count+1;
	//server.send(message, 0, message.length, rinfo.port, rinfo.address, function(err, bytes) {} );
	//console.log('Timeout ran at ' + new Date().toTimeString()+'\r\n');
});

server.on("listening", function () {
  var address = server.address();
  console.log("server listening " +
      address.address + ":" + address.port);
	  setTimeout(function() {
   console.log('Timeout ran at ' + new Date().toTimeString()+'num of packat:'+count+'\r\n');
   server.close();
}, 5000);
});

server.bind(5000);
