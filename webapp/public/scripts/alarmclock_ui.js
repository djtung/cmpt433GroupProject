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

	// socket.on('cmdReply', function(res) {
	//         var cmds = res.split('\n');
	//         for (var i = 0; i < cmds.length; ++i) {
	//                 var strs = cmds[i].split('=');
	//                 if (strs[0] === 'mode') {
	//                         var mode = 'error';
	//                         if (strs[1] === '0') {
	//                                 mode = 'None';
	//                         } else if (strs[1] === '1') {
	//                                 mode = 'Rock #1';
	//                         } else if (strs[1] === '2') {
	//                                 mode = 'Custom Beat';
	//                         }
	//                         $('#modeid').text(mode);
	//                 } else if (strs[0] === 'volume') {
	//                         $('#volumeid').val(strs[1]);
	//                 } else if (strs[0] === 'bpm') {
	//                         $('#tempoid').val(strs[1]);
	//                 }
	//         }
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

// Push alarms to Beaglebone to set
function sendBBGAlarms() {
	var msg;
	msg = '';
	$('#set-alarms-list').children('div').each(function(i) {
		var data = $(this).text();
		data = str.replace(/[\/: ]+/,',');

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

	return $.isNumeric(day) && $.isNumeric(month) && $.isNumeric(year) && $.isNumeric(min) && $.isNumeric(hour);
}

// Wrap a string in a new <div> tag
function divMessage(inString) {
	return $('<div></div>').text(inString);
}
