"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	$('#btnBeat0').click(function(){
		sendCommandViaUDP("beat0");
	});
	$('#btnBeat1').click(function(){
		sendCommandViaUDP("beat1");
	});
	$('#btnBeat2').click(function(){
		sendCommandViaUDP("beat2");
	});
	$('#btnVolMinus').click(function(){
		sendCommandViaUDP("vol-down");
	});
	$('#btnVolPlus').click(function(){
		sendCommandViaUDP("vol-up");
	});
	$('#btnTempoMinus').click(function(){
		sendCommandViaUDP("bpm-down");
	});
	$('#btnTempoPlus').click(function(){
		sendCommandViaUDP("bpm-up");
	});
	$('#btnBass').click(function(){
		sendCommandViaUDP("bass");
	});
	$('#btnHiHat').click(function(){
		sendCommandViaUDP("hihat");
	});
	$('#btnSoftSnare').click(function(){
		sendCommandViaUDP("softsnare");
	});
	$('#btnHardSnare').click(function(){
		sendCommandViaUDP("hardsnare");
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