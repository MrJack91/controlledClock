


//Function gets called each second
CoolClock.prototype.setTime = function(timeObj) {
		this.currentTime = timeObj;
};

CoolClock.prototype.refreshDisplay = function() {
		if(this.currentTime != undefined){
			this.currentTime.tic();
			this.render(this.currentTime.hour,this.currentTime.minute,this.currentTime.second);
		}
	};
