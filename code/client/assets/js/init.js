var myCoolClock;
var timeoutSuccessNotify = 0;

$(document).ready(function () {
  // set for jquery ui swissgerman as default
  $.datepicker.setDefaults($.datepicker.regional['de']);

  // register jquery ui
  $('#inputDate').datepicker(
    {
      defaultDate: '+0'
    }
  );
  // set prefilled date
  $('#inputDate').datepicker('setDate', '+0');

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

  myCoolClock.tickDelay = 100;

  // expand coolclock
  myCoolClock.expandClock();
  myCoolClock.setTime(new Date().getTime());

  $('#btnSetTimeManual').on('click', setTimeManual);
  $('#btnSyncTime').on('click', fireSyncTimeDcf77);
  $('#cbAutoSync').on('change', setupAutoSync);

  // init date fields with current values
  var curDate = new Date();
  var fullDate = pad(curDate.getDate(),2) + '' + '.' + curDate.getFullYear();
  /*
   $('#inputDate').attr('placeholder', fullDate);
   */
  var fullTime = pad(curDate.getHours(), 2) + ':' + pad(curDate.getMinutes(), 2) + ':00';
  $('#inputTime').val(fullTime);
  $('#inputTime').attr('placeholder', fullTime);
});


function fireSyncTimeDcf77(e) {
  syncTimeDcf77();
  e.preventDefault();
  return false;
}

function syncTimeDcf77(){
  var timeSend = new Date();
  setSyncState('run');
  $.ajax({
    url: "http://localhost:7899/",
    type: "GET",
    crossDomain: true,
    success: function (response) {
      var resp = JSON.parse(response);
      var curDate = new Date(resp.CurrentTime);
      if (isNaN(curDate.getDate())) {
        curDate = new Date(0);
      }
      var syncDate = new Date(resp.LastSyncTime);
      /*
      if (isNaN(syncDate.getDate())) {
        syncDate = new Date(0);
      }
      */
      // ntp
      var receiveTime = new Date();
      var totalTransferTime = receiveTime.getTime() - timeSend.getTime();
      var transferTimeSingleWay = totalTransferTime / 2;

      myCoolClock.setTime(curDate.getTime() - transferTimeSingleWay);

      // check the dates
      var syncDateTime = syncDate.toLocaleString();
      if (isNaN(syncDate.getDate())) {
        syncDateTime = 'Invalid Date (' + resp.LastSyncTime + ')';
        console.log(syncDateTime);
        syncDateTime = '-';
      }

      $('#spLastSyncServer').html(curDate.toLocaleString());
      $('#spLastSyncDCF77').html(syncDateTime);
      setSyncState('success');
      timeoutSuccessNotify = setTimeout(fadeOutSyncNotify, 800);
    },
    error: function (xhr, status) {
      setSyncState('error');
      console.log(xhr);
      console.log(status);
    }
  });
}

function fadeOutSyncNotify() {
  $('#spSyncSuccess').fadeOut(300);
  timeoutSuccessNotify = setTimeout('setSyncState(\'idle\')', 300);
}

function setTimeManual(e) {
  var sDate = $('#inputDate').val();
  var sTime = $('#inputTime').val();
  var arrDate = sDate.split('.');
  var arrTime = sTime.split(':');

  arrTime[2] = parseInt(arrTime[2]);
  if (isNaN(arrTime[2])) {
    arrTime[2] = 0;
  }

  // fix month
  var monthForDate = (parseInt(arrDate[1])+11)%12;
  var newDate = new Date(arrDate[2], monthForDate, arrDate[0], arrTime[0], arrTime[1], arrTime[2], 0);
  if (newDate.getFullYear() == arrDate[2]) {
    // no errors
    $('#fgDate').removeClass('has-error');
    $('#fgTime').removeClass('has-error');
    console.log(newDate + 'no error');
  } else {
    // error
    console.log('there was an error');
    $('#fgDate').addClass('has-error');
    $('#fgTime').addClass('has-error');
  }


  myCoolClock.setTime(newDate.getTime());

  e.preventDefault();
  return false;
}

function setupAutoSync(e){
  if($('#cbAutoSync').is(":checked")){
    myCoolClock.autoTimerId = setInterval(syncTimeDcf77, 1000);
  }else{
    clearInterval(myCoolClock.autoTimerId);
  }
}

function setSyncState(newState) {
  // clear timeout if there is any
  if (timeoutSuccessNotify > 0) {
    clearTimeout(timeoutSuccessNotify);
    timeoutSuccessNotify = 0;
  }

  // hide all states
  $('.labelSync').hide();
  // show new state
  var eleState = '';
  switch (newState) {
    case 'idle':
      eleState = 'spSyncIdle';
      break;
    case 'run':
      eleState = 'spSyncRun';
      break;
    case 'success':
      eleState = 'spSyncSuccess';
      break;
    case 'error':
      eleState = 'spSyncError';
      break;
  }
  $('#'+eleState).fadeIn();
}

function pad(num, size) {
  var s = "0000" + num;
  return s.substr(s.length-size);
}