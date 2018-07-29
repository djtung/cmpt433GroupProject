"use strict";

var socket = io.connect();

$(document).ready(function() {

	// poll for system uptime
	setInterval(function() {
		socket.emit('uptime', 0);
	}, 1000);

	$('#new-submit').click(function() {
		readUserInput();
	});

	$('#set-alarms').click(function() {
		sendBBGAlarms();
	});

	$('#clear-alarms').click(function() {
		clearAlarmsSet();
	});

	$('#google-calendar').click(function() {
		socket.emit('googlecal', '');
	});

	// event handlers
	// $('#alarm-music').click(function() {
	//         sendAlarmChange(0);
	// });

	// $('#alarm-tts').click(function() {
	//         sendAlarmChange(1);
	// });

	// $('#set-alarms').click(function() {
	//         sendBBGMessage(0);
	// });

	// $('#get-alarms').click(function() {
	//         sendBBGMessage(1);
	// });

	// $('#push-alarms-to-set').click(function() {
	//     pushAlarmsToSet();
	// });

	// $('#new-submit').click(function() {
	//         sendBBGMessage(2);
	// });

	// $('#push-alarms-to-set').click(function() {
	//         sendBBGMessage(3);
	// });

	socket.on('uptimeReply', function(res) {
		var str = res;
		var result = str.split(" ", 1);

		// Changing number of seconds into hours, minutes, and seconds format
		var seconds = parseInt(result, 10);
		
		// const SECONDS_IN_HOUR = 3600;
		var hour = Math.floor(seconds / 3600);
		seconds -= hour * 3600;
		// const SECONDS_IN_MIN = 60;
		var minute = Math.floor(seconds/ 60);
		seconds -= minute * 60;
		
		var final_output = hour + ":" + minute + ":" + seconds + " (HH:MM:SS)";

		$('#status-uptime').text(final_output);
	});

	socket.on('errorMsg', function(res) {
		$('#error-text').text(res);
		$('#error-box').show();
	});

	socket.on('googlecalReply', function(res) {
		console.log(res);
	});
});

function sendAlarmChange(mode) {
	var msg;
	if (mode === 0) {
		msg = 'mode=0';
	} else if (mode === 1) {
		msg = 'mode=1';
	} else {
		return;
	}

	socket.emit('cmd', msg);
}

function clearAlarmsSet() {
	$('#set-alarms-list').text('');
}

// Push alarms to Beaglebone to set
function sendBBGAlarms() {
	var msg;
	msg = '';
	$('#set-alarms-list').children('div').each(function(i) {
		var data = $(this).text();
		data = data.replace(/[\/: ]+/g,' ');

		msg += "alarm=" + data + "\n";
	});

	socket.emit('cmd', msg);
}

function readUserInput() {
	// Get the user's input from the browser
	var date = $('#new-date').val();
	var time = $('#new-time').val();
	var message = date + ' ' + time;

	console.log("from read user input:" + date);
	console.log("from read user input:" + time);

	if (checkFormat(date, time)) {
		// Display the command in the message list
		$('#set-alarms-list').append(divMessage(message));
	} else {
		alert('Please enter correct date and time!');
	}

	// Clear the user's command (ready for the next command)
	$('#new-date').val('');
	$('#new-time').val('');
}

// Date = (MM/DD/YYYY)
// Time = (HH:mm)
function checkFormat(date, time) {
	var day = date.substr(3,2);
	var month = date.substr(0,2);
	var year = date.substr(6);

	var hour = time.substr(0,2);
	var min = time.substr(3,2);

	console.log("day: " + day + '\nmonth: ' + month + '\nyear: ' + year);
	console.log("hour: " + hour + '\nminute: ' + min);

	if (month > 12 || month < 1 || year < 1 || day > 31 || hour > 24 || hour < 1 || min < 1 || min > 59) return false;

	return $.isNumeric(day) && $.isNumeric(month) && $.isNumeric(year) && $.isNumeric(min) && $.isNumeric(hour) && /\d{2}\:\d{2}/.test(time) && /\d{2}\/\d{2}\/\d{4}/.test(date);
}

// Wrap a string in a new <div> tag
function divMessage(inString) {
	return $('<div></div>').text(inString);
}
