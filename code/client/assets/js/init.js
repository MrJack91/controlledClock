
$(document).ready(function () {

//Create CoolClock

var myCoolClock = new CoolClock({
				canvasId:       'clockid',
				skinId:         'swissRail',
				displayRadius:  180,
				showSecondHand: 'noSeconds',
				gmtOffset:      2,
				showDigital:    true,
				logClock:       false,
				logClockRev:    false
			});

// Assign handlers immediately after making the request,
// and remember the jqxhr object for this request
var jqxhr = $.get( "http://localhost:7899/", function(data) {
	alert(data);
})
.done(function() {
	alert('done');
})
.fail(function(xhr, status, error) {
	alert('failed');
})
.always(function() {

});


var myTime = new TimeObject(2014,5,16,12,0,0);
myCoolClock.setTime(myTime);

});
