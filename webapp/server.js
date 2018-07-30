var PORT = 12345;

var http = require('http');
var fs   = require('fs');
var path = require('path');
var mime = require('mime');

const readline = require('readline');
const {google} = require('googleapis');

// If modifying these scopes, delete credentials.json.
const SCOPES = ['https://www.googleapis.com/auth/calendar.readonly'];
const TOKEN_PATH = 'token.json';

// Load client secrets from a local file.
fs.readFile('credentials.json', (err, content) => {
  if (err) return console.log('Error loading client secret file:', err);
  // Authorize a client with credentials, then call the Google Calendar API.
  authorize(JSON.parse(content), listEvents);
});

/**
 * Create an OAuth2 client with the given credentials, and then execute the
 * given callback function.
 * @param {Object} credentials The authorization client credentials.
 * @param {function} callback The callback to call with the authorized client.
 */
function authorize(credentials, callback) {
  const {client_secret, client_id, redirect_uris} = credentials.installed;
  const oAuth2Client = new google.auth.OAuth2(
	  client_id, client_secret, redirect_uris[0]);

  // Check if we have previously stored a token.
  fs.readFile(TOKEN_PATH, (err, token) => {
	if (err) return getAccessToken(oAuth2Client, callback);
	oAuth2Client.setCredentials(JSON.parse(token));
	callback(oAuth2Client);
  });
}

/**
 * Get and store new token after prompting for user authorization, and then
 * execute the given callback with the authorized OAuth2 client.
 * @param {google.auth.OAuth2} oAuth2Client The OAuth2 client to get token for.
 * @param {getEventsCallback} callback The callback for the authorized client.
 */
function getAccessToken(oAuth2Client, callback) {
  const authUrl = oAuth2Client.generateAuthUrl({
	access_type: 'offline',
	scope: SCOPES,
  });
  console.log('Authorize this app by visiting this url:', authUrl);
  const rl = readline.createInterface({
	input: process.stdin,
	output: process.stdout,
  });
  rl.question('Enter the code from that page here: ', (code) => {
	rl.close();
	oAuth2Client.getToken(code, (err, token) => {
	  if (err) return callback(err);
	  oAuth2Client.setCredentials(token);
	  // Store the token to disk for later program executions
	  fs.writeFile(TOKEN_PATH, JSON.stringify(token), (err) => {
	if (err) console.error(err);
	console.log('Token stored to', TOKEN_PATH);
	  });
	  callback(oAuth2Client);
	});
  });
}

/**
 * Lists the next 10 events on the user's primary calendar.
 * @param {google.auth.OAuth2} auth An authorized OAuth2 client.
 */
function listEvents(auth) {
  const calendar = google.calendar({version: 'v3', auth});
  calendar.events.list({
	calendarId: 'primary',
	timeMin: (new Date()).toISOString(),
	maxResults: 10,
	singleEvents: true,
	orderBy: 'startTime',
  }, (err, res) => {
	if (err) return console.log('The API returned an error: ' + err);
	const events = res.data.items;
	// Clear fiel
	fs.writeFile('public/google-calendar.txt','', function(err) {
			if (err) return console.log(err);
			console.log('Cleared google-calendar.txt');
		})
	if (events.length) {
		console.log('Upcoming 10 events:');
		events.map((event, i) => {
		const start = event.start.dateTime || event.start.date;
		// ${event.summary}
		fs.appendFile('public/google-calendar.txt', `${start}\n`, function(err) {
			if (err) return console.log(err);
			console.log('Google calendar contents > google-calendar.txt');
		})
		console.log(`${start} - ${event.summary}`);
	  });
	} else {
	  console.log('No upcoming events found.');
	}
  });
}

var app = http.createServer(function(req, res) {
	var fPath = false;

	if (req.url == '/') {
		fPath = 'public/index.html';
	} else {
		fPath = 'public' + req.url;
	}

	var absPath = './' + fPath;
	serveStatic(res, absPath);
});

app.listen(PORT, function() {
	console.log('Server listening on port ' + PORT);
});

function serveStatic(res, fPath) {
	fs.exists(fPath, function(exists) {
		if (exists) {
			fs.readFile(fPath, function(err, data) {
				if (err) {
					send404(res);
				} else {
					sendFile(res, fPath, data);
				}
			});
		} else {
			send404(res);
		}
	});
}

function send404(res) {
	res.writeHead(404, {'Content-Type': 'text/plain'});
	res.write('Error 404: resource not found.');
	res.end();
}

function sendFile(res, fPath, contents) {
	res.writeHead(200, {'content-type': mime.lookup(path.basename(fPath))});
	res.end(contents);
}

var udpServer = require('./lib/udp_server');
udpServer.listen(app);
