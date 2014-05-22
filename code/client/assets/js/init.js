var myCoolClock;

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

  // expand coolclock
  myCoolClock.expandClock();
  myCoolClock.setTime(0);

  $('#btnSetTimeManual').on('click', setTimeManual);
  $('#btnSyncTime').on('click', syncTimeDcf77);

  // init date fields with current values
  var curDate = new Date();
  var fullDate = curDate.getDay()+'.' + curDate.getDate() + '.' + curDate.getFullYear();
  // $('#inputDate').val(fullDate);
  $('#inputTime').val(curDate.getHours() + ':' + curDate.getMinutes());
});


function syncTimeDcf77(e) {
  var timeSend = new Date();
  $.ajax({
    url: "http://localhost:7899/",
    type: "GET",
    crossDomain: true,
    success: function (response) {
      var resp = JSON.parse(response);
      var curDate = new Date(resp.CurrentTime);
      var syncDate = new Date(resp.LastSyncTime);
      // ntp
      var receiveTime = new Date();
      var totalTransferTime = receiveTime.getTime() - timeSend.getTime();
      var transferTimeSingleWay = totalTransferTime / 2;

      myCoolClock.setTime(curDate.getTime() - transferTimeSingleWay);

      // check the dates
      var syncDateTime = syncDate.toLocaleString();
      console.log(syncDateTime);
      if (syncDate !== true) {
        syncDateTime = 'Invalid Date (' + resp.LastSyncTime + ')';
      }

      $('#spLastSyncServer').html(curDate.toLocaleString());
      $('#spLastSyncDCF77').html(syncDateTime);
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

function setTimeManual(e) {
  var sDate = $('#inputDate').val();
  var sTime = $('#inputTime').val();

  var newDate = new Date(sDate+ ' ' + sTime);

  console.log(newDate);

  myCoolClock.setTime(newDate.getTime());

  e.preventDefault();
  return false;
}