"use strict";
// Client-side interactions with the browser.

// Make connection to server when web page is fully loaded.
var socket = io.connect();
$(document).ready(function() {

	window.setInterval(function() {sendRequest('uptime')}, 1000);
	window.setInterval(function() {sendCommandViaUDP("getInfo");}, 1000);

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
		try {
			//will only work when pulling data from app
			var jsonObject = JSON.parse(result);
			var domObj_mode = $('#current-beat');
			var domObj_volume = $('#volumeText');
			var domObj_tempo = $('#tempoText');
			domObj_mode.html(jsonObject.mode);
			domObj_volume.val(jsonObject.volume);
			domObj_tempo.val(jsonObject.BPM);
		} catch (error) {
			console.log(result);
		}
		$('#error-box').css("display", "none");
	});

	socket.on('errorReply', function(result) {
		$('#error-message').html(result);
		$('#error-box').css("display", "block");
	});

	socket.on('fileContents', function(result) {
		var fileName = result.fileName;
		var contents = result.contents;
		// console.log(result);
		var domObj;
		switch(fileName) {
		case 'uptime':
			domObj = $('#hours');
			break;
		default:
			console.log("Unknown DOM object: " + fileName);
			return;
		}
		// Make linefeeds into <br> tag.
		console.log(contents);
		contents = replaceAll(contents, "\n", "<br/>");
		console.log(contents);
		domObj.html(contents);
	});
	
});

function sendCommandViaUDP(message) {
	socket.emit('daUdpCommand', message);
};

function sendRequest(file) {
	console.log("Requesting '" + file + "'");
	socket.emit('proc', file);
}

function replaceAll(str, find, replace) {
	return str.replace(new RegExp(find, 'g'), replace);
}