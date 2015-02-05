function  espconnect()
{
	console.log("Hello World");

	var http = require("http");


	var user={
	  "ap":
		{
		  "name": "default",
		  "password": "pass@word1234"
		}
	};
	var jsondata= {
		"Request": {
			"Softap": {
				"Connect_Softap": {
					"authmode":"WPAPSK/WPA2PSK",
					"channel":1,
					"ssid":"es82666",
					"password":"12345"
				}
			}
		}
	};

	var userString = JSON.stringify(user);

	console.log(userString);


	var head = {
	  'Content-Type': 'application/json',
	  'Content-length': userString.length
	};

	console.log(head);

	var options = {
	  hostname: '192.168.1.5',
	  //port: 3000,
	  path: '/ap?info=wifi',
	  //path: '/config?command=wifi',
	  //method: 'GET',
	  method:'POST',
	  headers: head
	};

	 var req = http.request(options, function(res) {
	  console.log('STATUS: ' + res.statusCode);
	  console.log('HEADERS: ' + JSON.stringify(res.headers));
	  res.setEncoding('utf8');
	/*   res.on('data', function (chunk) {
		console.log('BODY: ' + chunk);
	  }); */
	  
	  var responseString = '';

	  res.on('data', function(data) {
		responseString += data;
	  });

	  res.on('end', function() {
		 console.log('BODY: ' + responseString);
		//var resultObject = JSON.parse(responseString);
		//console.log('BODY: ' + resultObject);
	  })
	  
	});


	req.on('error', function(e) {
	  console.log('problem with request: ' + e.message);
	});

	// write data to request body
	req.write(userString);
	req.end();
}
espconnect();
// set function to be called after 1 second
setTimeout(function() {
   console.log('Timeout ran at ' + new Date().toTimeString());
}, 5000);

// store the start time
var start = new Date();
console.log('Enter loop at: '+start.toTimeString());

