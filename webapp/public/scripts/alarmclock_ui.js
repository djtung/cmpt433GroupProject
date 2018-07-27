"use strict";

var socket = io.connect();
$(document).ready(function() {

        // poll for system uptime
        setInterval(function() {
                socket.emit('uptime', 0);
        }, 1000);

        $('#set-date').focus();

        // event handlers
        $('#alarm-music').click(function() {
                sendAlarmChange(0);
        });
        $('#alarm-tts').click(function() {
                sendAlarmChange(1);
        });

        $('#set-alarms').click(function() {
                sendBBGMessage(0);
        });

        $('#get-alarms').click(function() {
                sendBBGMessage(1);
        });

        $('#new-submit').submit(function() {
                readUserInput();

                // Return false to show that we have handled it
                return false;
        });

        $('#new-submit').click(function() {
                sendBBGMessage(2);
        });

        $('#push-alarms-to-set').click(function() {
                sendBBGMessage(3);
        });

        socket.on('cmdReply', function(res) {
                var cmds = res.split('\n');
                for (var i = 0; i < cmds.length; ++i) {
                        var strs = cmds[i].split('=');
                        if (strs[0] === 'mode') {
                                var mode = 'error';
                                if (strs[1] === '0') {
                                        mode = 'None';
                                } else if (strs[1] === '1') {
                                        mode = 'Rock #1';
                                } else if (strs[1] === '2') {
                                        mode = 'Custom Beat';
                                }
                                $('#modeid').text(mode);
                        } else if (strs[0] === 'volume') {
                                $('#volumeid').val(strs[1]);
                        } else if (strs[0] === 'bpm') {
                                $('#tempoid').val(strs[1]);
                        }
                }
        });

        socket.on('uptimeReply', function(res) {
          console.log(res + "\n");
          var str = res;
          var result = str.split(" ", 1);

          console.log(result + "\n");

		// Changing number of seconds into hours, minutes, and seconds format
		var seconds = parseInt(result, 10);
		
		// const SECONDS_IN_HOUR = 3600;
		var hour = Math.floor(seconds / 3600);
		seconds -= hour * 3600;
		// const SECONDS_IN_MIN = 60;
		var minute = Math.floor(seconds/ 60);
		seconds -= minute * 60;
		
		var final_output = hour + ":" + minute + ":" + seconds + " (HH:MM:SS)";

		$('#status').text(final_output);
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

function sendBBGMessage(type) {
        var msg;
        if (type === 0) {
                // Push alarms to Beaglebone to set
                msg = 'set_alarms';
        } else if (type === 1) {
                // Beaglebone sends information back on alarms already set
                msg = 'get_already_set'
        }
}

function readUserInput() {
        // Get the user's uniput from the browser
        var date = $('#new-date').val();
        var time = $('#new-time').val();
        var message = date + ' ' + time;

        // Display the command in the message list
        $('#list-alarms').append(divMessage(message));

        // Process the command
        var systemMessage = processCommand(message);

        // Clear the user's command (ready for the next command)
        $('#new-date').val('');
        $('#new-time').val('');
}

function processCommand(command) {
        var words = command.split(' ');

        // Convert arguments to numbers
        var date = Number(words[0]);
        var time = Number(words[1]);

        // Put numbers into a custom structure to send to server
        var message = {
                alarm-date: date,
                alarm-time: time
        };
        socket.emit('setAlarm', message);
}

// Wrap a string in a new <div> tag
function divMessage(inString) {
        return $('<div></div>').text(inString);
}
