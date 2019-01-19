send = function ()
{
if ("WebSocket" in window) {
  var ws = new WebSocket("ws://localhost:8888/");
  ws.onopen = function() {
    // Web Socket is connected. You can send data by send() method.
    ws.send("message to send"); 
  };
  ws.onmessage = function (evt) { var received_msg = evt.data; ... };
  ws.onclose = function() { // websocket is closed. };
} else {
  alert("the browser doesn't support WebSocket.");
}
}