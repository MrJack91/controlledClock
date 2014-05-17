
$(document).ready(function () {

//Enable Cross-Domain-Support
$.support.cors = true;

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
/*var jqxhr = $.get( "http://localhost:7899/", function(data) {
	var resp = JSON.parse(data)
    alert(resp.status);
})
.done(function() {
	alert('done');
})
.fail(function(xhr, status, error) {
	alert('failed');
})
.always(function() {

});
$.ajax({
            url: "http://localhost:7899/",
            type: "GET",
            crossDomain: true,
            success: function (response) {
                var resp = JSON.parse(response)
                alert(resp.status);
            },
            error: function (xhr, status) {
                alert("error");
            }
        });*/


var myTime = new TimeObject(2014,5,16,12,0,0);
myCoolClock.setTime(myTime);

	
});
