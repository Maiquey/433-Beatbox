"use strict";
/*
 * Respond to commands over a websocket to relay UDP commands to a local program
 */

// Large portion adapted from udpServer example

var fs   = require('fs');
var socketio = require('socket.io');
var io;

var dgram = require('dgram');

exports.listen = function(server) {
	io = socketio.listen(server);
	io.set('log level 1');

	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {
	// Pased string of comamnd to relay
	socket.on('udpCommand', function(data) {

		// Info for connecting to the local process via UDP
		var PORT = 12345;
		var HOST = '127.0.0.1';
		var buffer = new Buffer(data);

		var client = dgram.createSocket('udp4');

		var timeout = setTimeout(function() { //tiny bit of help from GPT here :)
			var errMsg = "No response from beat-box application. Is it running?"
			socket.emit('errorReply', errMsg);
			client.close();
		}, 1000);

		client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
			if (err) 
				throw err;
		});

		client.on('listening', function () {
			var address = client.address();
		});
		// Handle an incoming message over the UDP from the local application.
		client.on('message', function (message, remote) {

			var reply = message.toString('utf8')
			socket.emit('commandReply', reply);
			clearTimeout(timeout);
			client.close();

		});
		client.on("UDP Client: close", function() {
			console.log("closed");
			clearTimeout(timeout);
		});
		client.on("UDP Client: error", function(err) {
			clearTimeout(timeout);
			console.log("error: ",err);
		});
	});
				// socket.emit('errorReply', something);

	socket.on('proc', function(fileName) {
		// NOTE: Very unsafe? Why?
		// Hint: think of ../
		var absPath = "/proc/" + fileName;
		
		fs.exists(absPath, function(exists) {
			if (exists) {
				// Can use 2nd param: 'utf8', 
				fs.readFile(absPath, function(err, fileData) {
					if (err) {
						emitSocketData(socket, fileName, 
								"ERROR: Unable to read file " + absPath);
					} else {
						emitSocketData(socket, fileName, 
								fileData.toString('utf8'));
					}
				});
			} else {
				emitSocketData(socket, fileName, 
						"ERROR: File " + absPath + " not found.");
			}
		});
	});
};

function emitSocketData(socket, fileName, contents) {
	var result = {
			fileName: fileName,
			contents: contents
	}
	socket.emit('fileContents', result);	
}