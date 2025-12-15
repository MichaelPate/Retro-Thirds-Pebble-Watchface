/**
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
*/

// This is where we build the XML Http request that we will send to OpenWeatherMap.org
// The API key is not included here, for that we put it in the URL
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// API key stored in secrets file so its not shown on github
const secrets = require("./secrets.js");
if (!secrets.OWM_KEY)
{
  console.log("Missing OpenWeatherMap API key");
}
const myAPIKey = secrets.OWM_KEY;

function locationSuccess(pos) {
  // Construct URL
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', function(responseText) {
    // responseText contains a JSON object with weather info
    var json = JSON.parse(responseText);

    // Temperature in Kelvin requires adjustment
    var temperature = Math.round(json.main.temp - 273.15);
    var tempMin = Math.round(json.main.temp_min - 273.15);
    var tempMax = Math.round(json.main.temp_max - 273.15);
    //console.log('Temperature is ' + temperature);

    // Conditions
    var conditions = json.weather[0].main;
    var conditionsDesc = json.weather[0].description;
    //console.log('Conditions are ' + conditions);

    // Assemble dictionary using our keys
    var dictionary = {
      'TEMPERATURE': temperature,
      'TEMPERATUREMIN': tempMin,
      'TEMPERATUREMAX': tempMax,
      'CONDITIONS': conditions,
      'CONDITIONSDESC': conditionsDesc
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary, function(e) {
      console.log('Weather info sent to Pebble successfully!');
    }, function(e) {
      console.log('Error sending weather info to Pebble!');
    });
  });
}


function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  if ("geolocation" in navigator)
  {
    navigator.geolocation.getCurrentPosition(
      locationSuccess,
      locationError,
      {timeout: 15000, maximumAge: 60000} // units are seconds
    );
  }
  else
  {
    console.log("Geolocation is not supported by this browser.");
  }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');

  // Get the initial weather
  getWeather();
});

// Listen for when an AppMessage is received
// By just calling getWeather() we can just send an empty message
// no need to parse its contents. If we need to do other function here
// then we will need to change that logic
Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received!');

  // Get updated weather now
  getWeather();
});