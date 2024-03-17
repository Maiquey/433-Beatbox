"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	$('#btnBeat0').click(function(){
		sendCommandViaUDP("help");
	});
	$('#btnBeat1').click(function(){
		sendCommandViaUDP("?");
	});
	$('#btnBeat2').click(function(){
		sendCommandViaUDP("length");
	});
	$('#btnHistory').click(function(){
		sendCommandViaUDP("history");
	});
	$('#btnTerminate').click(function(){
		sendCommandViaUDP("stop");
	});
	
	socket.on('commandReply', function(result) {
		console.log(result);
		// var newDiv = $('<code></code>')
		// 	.text(result)
		// 	.wrapInner("<div></div>");
		// $('#messages').append(newDiv);
		// $('#messages').scrollTop($('#messages').prop('scrollHeight'));
	});
	
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};