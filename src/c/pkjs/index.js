// Listen for when the watchface is opened
// ready fires when the JS environment on the phone is first ready after launch
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
});

// Listen for when an AppMessage is received
// appmessage fires whenever an appmessage is sent from the watch to the phone
Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');
});