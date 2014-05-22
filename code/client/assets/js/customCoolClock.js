
CoolClock.prototype.expandClock = function() {
  CoolClock.prototype.currentSeconds = 0;

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
    this.render(oTime.getHours(),oTime.getMinutes(),oTime.getSeconds());
  }
}