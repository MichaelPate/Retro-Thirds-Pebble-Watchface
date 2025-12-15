/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId])
/******/ 			return installedModules[moduleId].exports;
/******/
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			exports: {},
/******/ 			id: moduleId,
/******/ 			loaded: false
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.loaded = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(0);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, exports, __webpack_require__) {

	__webpack_require__(1);
	module.exports = __webpack_require__(2);


/***/ }),
/* 1 */
/***/ (function(module, exports) {

	/**
	 * Copyright 2024 Google LLC
	 *
	 * Licensed under the Apache License, Version 2.0 (the "License");
	 * you may not use this file except in compliance with the License.
	 * You may obtain a copy of the License at
	 *
	 *     http://www.apache.org/licenses/LICENSE-2.0
	 *
	 * Unless required by applicable law or agreed to in writing, software
	 * distributed under the License is distributed on an "AS IS" BASIS,
	 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 * See the License for the specific language governing permissions and
	 * limitations under the License.
	 */
	
	(function(p) {
	  if (!p === undefined) {
	    console.error('Pebble object not found!?');
	    return;
	  }
	
	  // Aliases:
	  p.on = p.addEventListener;
	  p.off = p.removeEventListener;
	
	  // For Android (WebView-based) pkjs, print stacktrace for uncaught errors:
	  if (typeof window !== 'undefined' && window.addEventListener) {
	    window.addEventListener('error', function(event) {
	      if (event.error && event.error.stack) {
	        console.error('' + event.error + '\n' + event.error.stack);
	      }
	    });
	  }
	
	})(Pebble);


/***/ }),
/* 2 */
/***/ (function(module, exports, __webpack_require__) {

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
	console.log("Hello");
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
	const secrets = __webpack_require__(3);
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
	    console.log('Temperature is ' + temperature);
	
	    // Conditions
	    var conditions = json.weather[0].main;
	    var conditionsDesc = json.weather[0].description;
	    console.log('Conditions are ' + conditions);
	
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
	  console.log('I say PebbleKit JS ready!');
	
	  // Get the initial weather
	  getWeather();
	});
	
	// Listen for when an AppMessage is received
	Pebble.addEventListener('appmessage', function(e) {
	  console.log('I saw AppMessage received!');
	
	  // Get updated weather now
	  getWeather();
	});

/***/ }),
/* 3 */
/***/ (function(module, exports) {

	//export const OWM_KEY = "3ffacd56700b1bae38173398a3287491";
	module.exports = { OWM_KEY: "3ffacd56700b1bae38173398a3287491" }

/***/ })
/******/ ]);
//# sourceMappingURL=pebble-js-app.js.map