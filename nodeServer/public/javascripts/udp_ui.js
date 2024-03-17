"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	$('#btnHelp').click(function(){
		sendCommandViaUDP("help");
	});
	$('#btnCount').click(function(){
		sendCommandViaUDP("count");
	});
	$('#btnLength').click(function(){
		sendCommandViaUDP("length");
	});
	$('#btnHistory').click(function(){
		sendCommandViaUDP("history");
	});
	$('#btnStop').click(function(){
		sendCommandViaUDP("stop");
	});
	
	socket.on('commandReply', function(result) {
		var newDiv = $('<code></code>')
			.text(result)
			.wrapInner("<div></div>");
		$('#messages').append(newDiv);
		$('#messages').scrollTop($('#messages').prop('scrollHeight'));
	});
	
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};