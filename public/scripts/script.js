window.setInterval(() => {
  nocache = "&nocache=" + Math.random() * 10;
  var request = new XMLHttpRequest();
  request.onreadystatechange = function () {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (this.responseText != null) {
          document.getElementById("sensor").innerHTML = this.responseText;
        }
      }
    }
  };
  request.open("GET", "ajaxrefresh" + nocache, true);
  request.send(null);
}, 2000);

moveUpBlackOut = () => {
  nocache = "&nocache=" + Math.random() * 10;
  var request = new XMLHttpRequest();
  request.onreadystatechange = function () {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (this.responseText != null) {
          document.getElementById("moveUp-indicator").innerHTML =
            this.responseText;
        }
      }
    }
  };
  request.open("GET", "?isBlackOutUp=1" + nocache, true);
  request.send(null);
};

moveDownBlackOut = () => {
  nocache = "&nocache=" + Math.random() * 10;
  var request = new XMLHttpRequest();
  request.onreadystatechange = function () {
    if (this.readyState == 4) {
      if (this.status == 200) {
        if (this.responseText != null) {
          document.getElementById("moveDown-indicator").innerHTML =
            this.responseText;
        }
      }
    }
  };
  request.open("GET", "?isBlackOutDown=1" + nocache, true);
  request.send(null);
};
