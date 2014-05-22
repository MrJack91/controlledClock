
CoolClock.prototype.expandClock = function() {
  CoolClock.prototype.currentSeconds = 0;
  CoolClock.prototype.monthNames = [ "Januar", "Februar", "März", "April", "Mai", "Juni", "July", "August", "September", "Oktober", "November", "Dezember" ]; 
  CoolClock.prototype.dayNames= ["Sonntag","Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag"]
  CoolClock.prototype.autoTimerId = -1;
  
  //Function gets called each second
  CoolClock.prototype.setTime = function(miliseconds) {
    CoolClock.prototype.currentSeconds = miliseconds;
  };

  CoolClock.prototype.refreshDisplay = function() {
    var oTime = null;
    if (CoolClock.prototype.currentSeconds > 0) {
      CoolClock.prototype.currentSeconds += 1000;
      oTime = new Date(CoolClock.prototype.currentSeconds);
    } else {
      oTime = new Date();
    }
	var seconds = oTime.getSeconds();
	var minutes = oTime.getMinutes();
	var hours = oTime.getHours();
	
	$("#sec").html(( seconds < 10 ? "0" : "" ) + seconds);
	$("#min").html(( minutes < 10 ? "0" : "" ) + minutes);
	$("#hours").html(( hours < 10 ? "0" : "" ) + hours);
	$('#digiClockDate').html(CoolClock.prototype.dayNames[oTime.getDay()] + " " + oTime.getDate() + ' ' + CoolClock.prototype.monthNames[oTime.getMonth()] + ' ' + oTime.getFullYear());
	
    this.render(hours,minutes,seconds);
  }
}