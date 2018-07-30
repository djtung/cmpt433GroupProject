"use strict"

var socketio = require('socket.io');
var dgram = require('dgram');
var fs = require('fs');
var io;

var PORT = 12345;
var HOST = '127.0.0.1';

exports.listen = function(server) {
	io = socketio.listen(server);

	io.sockets.on('connection', function(socket) {
		handleCommand(socket);
	});
};

function handleCommand(socket) {
	// relay control commands
	socket.on('cmd', function(data) {
		// 2 second timeout for internal udp requests
		var to = setTimeout(function() {
			console.log('Response not received from alarm clock application before timeout');
			socket.emit('errorMsg', 'No response from alarm clock application');
		}, 2000);
		console.log('Received a command:\n' + data);
		sendRequestToApp(socket, data, to);
	});

	// relay system uptime
	socket.on('uptime', function() {
		var fn = '/root/project/test.txt';
		fs.exists(fn, function(exists) {
			if (exists) {
				fs.readFile(fn, function(err, data) {
					if (err) {
						console.log('Unable to get system uptime');
						socket.emit('errorMsg', 'Unable to read /proc/uptime file');
					} else {
						var reply = data.toString('utf8');
						socket.emit('uptimeReply', reply);
					}
				});
			} else {
				socket.emit('errorMsg', '/proc/uptime file does not exist');
			}
		});

		// also poll the app for any settings changes
		sendRequestToApp(socket, 'poll', null);
	});
}

function sendRequestToApp(socket, data, to) {
	var buffer = new Buffer(data);

	var client = dgram.createSocket('udp4');
	client.send(buffer, 0, buffer.length, PORT, HOST, function(err, bytes) {
		if (err) {
			socket.emit('errorMsg', 'Unable to relay command to app');
			throw err;
		}
	});

	client.on('message', function(msg, remote) {
		var reply = msg.toString('utf8');
		socket.emit('cmdReply', reply);
		client.close();
		if (to)
			clearTimeout(to);
	});
}
