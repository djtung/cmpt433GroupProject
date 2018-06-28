var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res) {
  res.render('home', { title: 'Smart Alarm' });
});

router.get('/mycal', function(req, res){
  res.render('mycal', {
    title: 'Calendar'
  });
});

router.get('/alarms', function(req, res){
  res.render('alarms', {
    title: 'My Alarms'
  });
});

router.get('/ringtones', function(req, res){
  res.render('ringtones', {
    title: 'My Ringtone Library'
  });
});

module.exports = router;

// function openTab(evt, tabName) {
//     // Declare all variables
//     var i, tabcontent, tablinks;
//
//     // Get all elements with class="tabcontent" and hide them
//     tabcontent = document.getElementsByClassName("tabcontent");
//     for (i = 0; i < tabcontent.length; i++) {
//         tabcontent[i].style.display = "none";
//     }
//
//     // Get all elements with class="tablinks" and remove the class "active"
//     tablinks = document.getElementsByClassName("tablinks");
//     for (i = 0; i < tablinks.length; i++) {
//         tablinks[i].className = tablinks[i].className.replace(" active", "");
//     }
//
//     // Show the current tab, and add an "active" class to the button that opened the tab
//     document.getElementById(tabName).style.display = "block";
//     evt.currentTarget.className += " active";
// }
