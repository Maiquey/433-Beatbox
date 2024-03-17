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
	
});

function sendCommandViaUDP(message) {
	socket.emit('udpCommand', message);

	var timeout = setTimeout(function() { //tiny bit of help from GPT here :)
		var errMsg = "No response from back-end. Is NodeJS running on the target?";
		$('#error-message').html(errMsg);
		$('#error-box').css("display", "block");
	}, 1000);

	socket.on('commandReply', function(result) {
		clearTimeout(timeout);
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
			// console.log(result);
		}
		$('#error-box').css("display", "none");
	});

	socket.on('errorReply', function(result) {
		clearTimeout(timeout);
		$('#error-message').html(result);
		$('#error-box').css("display", "block");
	});
};

function sendRequest(file) {
	socket.emit('proc', file);

	var timeout = setTimeout(function() { //tiny bit of help from GPT here :)
		var errMsg = "No response from back-end. Is NodeJS running on the target?";
		$('#error-message').html(errMsg);
		$('#error-box').css("display", "block");
	}, 1000);

	socket.on('fileContents', function(result) {
		clearTimeout(timeout);
		var fileName = result.fileName;
		var contents = result.contents;
		if (fileName != 'uptime'){
			// console.log("Unknown DOM object: " + fileName);
			return;
		}
		var uptimeVals = contents.split(" ");
		var bbgUptime = parseInt(uptimeVals[0]);
		var seconds = bbgUptime % 60;
		var minutes = Math.floor(bbgUptime / 60);
		var hours = Math.floor(minutes / 60);
		minutes = minutes % 60;
		$('#hours').html(hours);
		$('#minutes').html(minutes);
		$('#seconds').html(seconds);
		$('#timeCheck').html(bbgUptime);
	});
}

function replaceAll(str, find, replace) {
	return str.replace(new RegExp(find, 'g'), replace);
}