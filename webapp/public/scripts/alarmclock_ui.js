"use strict";

var socket = io.connect();

$(document).ready(function() {

	// Poll every 5 seconds
	setInterval(function() {
		socket.emit('uptime', 0);
	}, 5000);

	$('#new-submit').click(function() {
		readUserInput();
	});

	$('#set-alarms').click(function() {
		sendBBGAlarms();
		clearAlarmsSet();
	});

	$('#clear-alarms').click(function() {
		clearAlarmsSet();
	});

	// data example: 2018-07-29T15:30:00-07:00 - CMPT 433 - test 4 2018-07-30T11:30:00-07:00 - CMPT 433 - test 5
	// Possible to remove the titles of the dates so it would change into:
	// 2018-07-29T15:30:00-07:00 2018-07-30T11:30:00-07:00
	$('#google-calendar').click(function() {
		$.get('google-calendar.txt', function(data) {
			var re = /(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2})/g;

			var years = getMatches(data, re, 1);
			var months = getMatches(data, re, 2);
			var days = getMatches(data, re, 3);
			var hours = getMatches(data, re, 4);
			var minutes = getMatches(data, re, 5);

			var result = "";
			var length = years.length;

			var i;
			for (i = 0; i < length; i++) {
				result = result + months[i] + "/" + days[i] + "/" + years[i] + " " + hours[i] + ":" + minutes[i] + "<br>" + "\n";
			}
			$('#set-alarms-list').append("<div>" + result + "</div>");
		});
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
		var unixTimes = res.split("\n");
		var finalString = "";
		var i = 0;

		for (i = 0; i < unixTimes.length-1; i++) {
			finalString += timeConverter(unixTimes[i]) + '\n' + '<br>';
		}

		$('#status-uptime').html(finalString);
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

		msg += data + "\n";
	});

	socket.emit('cmd', msg);
}

function readUserInput() {
	// Get the user's input from the browser
	var date = $('#new-date').val();
	var time = $('#new-time').val();
	var message = date + ' ' + time;

	//console.log("from read user input:" + date);
	//console.log("from read user input:" + time);

	if (checkFormat(date, time)) {
		// Display the command in the message list
		$('#set-alarms-list').append(divMessage(message));

		// Clear the user's command (ready for the next command)
		$('#new-date').val('');
		$('#new-time').val('');
	} else {
		alert('Please enter correct date and time!');
	}
}

// Date = (MM/DD/YYYY)
// Time = (HH:mm)
function checkFormat(date, time) {
	var day = date.substr(3,2);
	var month = date.substr(0,2);
	var year = date.substr(6);

	var hour = time.substr(0,2);
	var min = time.substr(3,2);

	//console.log("day: " + day + '\nmonth: ' + month + '\nyear: ' + year);
	//console.log("hour: " + hour + '\nminute: ' + min);

	// Perform checks
	if (month > 12 || month < 1 || year < 1 || day > 31 || hour > 24 || hour < 0 || min < 0 || min > 60) return false;

	return $.isNumeric(day) && $.isNumeric(month) && $.isNumeric(year) && $.isNumeric(min) && $.isNumeric(hour) && /\d{2}\:\d{2}/.test(time) && /\d{2}\/\d{2}\/\d{4}/.test(date);
}

// Wrap a string in a new <div> tag
function divMessage(inString) {
	return $('<div></div>').text(inString);
}

function getMatches(string, regex, index) {
  index || (index = 1); // default to the first capturing group
  var matches = [];
  var match;
  while (match = regex.exec(string)) {
    matches.push(match[index]);
  } 
  return matches;
}

// from https://stackoverflow.com/questions/847185/convert-a-unix-timestamp-to-time-in-javascript
function timeConverter(UNIX_timestamp){
	if (UNIX_timestamp !== "") {
		var a = new Date(UNIX_timestamp * 1000);
		var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
		var year = a.getFullYear();
		var month = months[a.getMonth()];
		var date = a.getDate();
		var hour = a.getHours();
		var min = a.getMinutes();

		if (min === 0) {
			min = "00";
		}
		if (min < 10 && min > 0) {
			min = "0" + min;
		}

		var time = month + ' ' + date + ' ' + year + ' ' + hour + ':' + min;
		return time;
	} else {
		return "";
	}
}