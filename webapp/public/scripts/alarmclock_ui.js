"use strict";

var socket = io.connect();
$(document).ready(function() {

        // poll for system uptime
        setInterval(function() {
                socket.emit('uptime', 0);
        }, 1000);

        // event handlers
        $('#modeNone').click(function() {
                sendModeChange(0);
        });
        $('#modeRock1').click(function() {
                sendModeChange(1);
        });
        $('#modeRock2').click(function() {
                sendModeChange(2);
        });
        $('#volumeDown').click(function() {
                sendVolumeChange(false);
        });
        $('#volumeUp').click(function() {
                sendVolumeChange(true);
        });
        $('#tempoDown').click(function() {
                sendTempoChange(false);
        });
        $('#tempoUp').click(function() {
                sendTempoChange(true);
        });
        $('#hihat').click(function() {
                sendPlaySound(2);
        });
        $('#snare').click(function() {
                sendPlaySound(0);
        });
        $('#bass').click(function() {
                sendPlaySound(1);
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

function sendModeChange(mode) {
        var msg;
        if (mode === 0) {
                msg = 'mode=0';
        } else if (mode === 1) {
                msg = 'mode=1';
        } else if (mode === 2) {
                msg = 'mode=2';
        } else {
                return;
        }

        socket.emit('cmd', msg);
}

function sendVolumeChange(vol) {
        var msg;
        if (vol) {
                msg = 'volume=1';
        } else {
                msg = 'volume=0';
        }

        socket.emit('cmd', msg);
}

function sendTempoChange(tempo) {
        var msg;
        if (tempo) {
                msg = 'bpm=1';
        } else {
                msg = 'bpm=0';
        }

        socket.emit('cmd', msg);
}

function sendPlaySound(sound) {
        var msg;
        if (sound === 0) {
                msg = 'play=0';
        } else if (sound === 1) {
                msg = 'play=1';
        } else if (sound === 2) {
                msg = 'play=2';
        } else {
                return;
        }

        socket.emit('cmd', msg);
}
