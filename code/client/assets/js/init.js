
$(document).ready(function () {

  //Enable Cross-Domain-Support
  $.support.cors = true;

  //Create CoolClock
  myCoolClock = new CoolClock({
    canvasId:       'clockid',
    skinId:         'swissRail',
    displayRadius:  150,
    showSecondHand: 'noSeconds',
    gmtOffset:      2,
    showDigital:    true,
    logClock:       false,
    logClockRev:    false
  });

  $('#btnSyncTime').on('click', syncTimeDcf77);


  var myTime = new TimeObject(2014,5,16,12,0,0);
  myCoolClock.setTime(myTime);


});


function syncTimeDcf77(e) {
  $.ajax({
    url: "http://localhost:7899/",
    type: "GET",
    crossDomain: true,
    success: function (response) {
      var resp = JSON.parse(response);
      console.log(resp.CurrentTime);
      console.log(resp.LastSyncTime);
    },
    error: function (xhr, status) {
      console.log(xhr);
      console.log(status);
      alert("error");
    }
  });
  e.preventDefault();
  return false;
}