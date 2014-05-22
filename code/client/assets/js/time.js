
var TimeObject = function(year, month, day, hour, minute, second){
  this.year = year;
  this.month = month;
  this.day = day;
  this.hour = hour;
  this.minute = minute;
  this.second = second;
}

TimeObject.prototype = {

  tic: function(){
    this.second++;

    if(this.second > 59){
      this.second = 0;
      this.minute++;

      if(this.minute > 59){
        this.minute = 0;
        this.hour++;

        if(this.hour > 23){
          this.hour = 0;
          this.day++;

          var maxDay = 31;

          switch (this.month) {
            case 4:
            case 6:
            case 9:
            case 11:
              maxDay = 30;
              break;
            case 2:
              if ((((this.year % 4) == 0) && !((this.year % 100) == 0)) || (((this.year % 4) == 0) && ((this.year % 400) == 0))) {
                maxDay = 29;
              } else {
                maxDay = 28;
              }
              break;
          }

          if(this.day > maxDay){
            this.day = 0;
            this.month++;

            if(this.month > 12){
              this.month = 1;
              this.year++;
            }
          }
        }
      }
    }
  }
};