var PORT = 8080;

var http = require('http');
var fs   = require('fs');
var path = require('path');
var mime = require('mime');

const express = require('express')
const app = express()

app.set('view engine', 'ejs')
app.use(express.static('public'));

app.get('/', function (req, res) {
  // res.send('Hello World!')
  res.render('index');
})

app.listen(PORT, function () {
  console.log('Example app listening on port 8080!')
})
