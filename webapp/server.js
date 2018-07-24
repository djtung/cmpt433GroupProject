var PORT = 12345;

var http = require('http');
var fs   = require('fs');
var path = require('path');
var mime = require('mime');

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
