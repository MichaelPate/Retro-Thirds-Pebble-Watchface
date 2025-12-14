#include <pebble.h>

// Window Objects
static Window *sMainWindow;

// Layer Objects, each line of text has its own layer
static TextLayer *sWeatherTextLayer, *sTemperatureTextLayer;
static TextLayer *sTimeTextLayer;
static TextLayer *sDayTextLayer, *sBattBtTextLayer;

static GFont sTimeFont, sTextFont;

// Our background color layers
static Layer *sBgTopLayer, *sBgBottomLayer; // no background for middle layer since time text just uses its BG color
#define top_y 54//PBL_IF_ROUND_ELSE(58, 52); // Need to go back
//const int middle_size = PBL_IF_ROUND_ELSE(58, 52);
#define bottom_y top_y + 58 // which y level will the layers be defined at
static GColor bgTopColor, bgMiddleColor, bgBottomColor;
static GColor weatherTextColor, timeTextColor, dateTextColor;

static int sBatteryLevel;
static bool sBTConnected;

// Required to set background color of the top and bottom thirds
static void topThirdBgColor_proc(Layer *layer, GContext *ctx)
{
  graphics_context_set_fill_color(ctx, bgTopColor);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}
static void bottomThirdBgColor_proc(Layer *layer, GContext *ctx)
{
  graphics_context_set_fill_color(ctx, bgBottomColor);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void updateBatteryAndBT()
{
  static char s_battery_buffer[16];

  if (sBTConnected)
  {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "Batt.%d%%", sBatteryLevel);
  }
  else
  {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "Batt.%d%% NoBT", sBatteryLevel);
  }
  

  text_layer_set_text(sBattBtTextLayer, s_battery_buffer);
}

// Normally this would go in the tick handler directly,
// but its its own function so we can also call it whenever we want (init)
static void updateTime()
{
  // Get a tm structure
  time_t tempTime = time(NULL);
  struct tm *tick_time = localtime(&tempTime);

  // Write the current hours and minutes into a buffer
  static char sTimeBuffer[8];
  strftime(sTimeBuffer, sizeof(sTimeBuffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(sTimeTextLayer, sTimeBuffer);

  // Now the same steps but for the day ("Sun. 12/14" for Sunday, December 14)
  static char sDateBuffer[12];
  strftime(sDateBuffer, sizeof(sDateBuffer), "%a. %m/%d", tick_time);
  text_layer_set_text(sDayTextLayer, sDateBuffer);
}

// When the main window first loads
static void main_window_load(Window *window)
{
  // Get information about the Window for the watchface
  Layer *window_layer = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(window_layer);

  // Initialize fonts
  sTimeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ABDUCTION_48));
  sTextFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PERFECT_DOS_18));

  // Create our background color layers and add them as well
  sBgTopLayer = layer_create(GRect(0, 0, windowBounds.size.w, top_y));
  //bgTopColor = GColorVeryLightBlue;
  bgTopColor = GColorVeryLightBlue;
  layer_set_update_proc(sBgTopLayer, topThirdBgColor_proc);
  layer_add_child(window_layer, sBgTopLayer);

  sBgBottomLayer = layer_create(GRect(0, bottom_y, windowBounds.size.w, windowBounds.size.h));
  bgBottomColor = GColorRoseVale;
  layer_set_update_proc(sBgBottomLayer, bottomThirdBgColor_proc);
  layer_add_child(window_layer, sBgBottomLayer);

  // Create the time TextLayer with specific bounds (in its third of the screen) and colors
  sTimeTextLayer = text_layer_create(
      GRect(0, top_y, windowBounds.size.w, bottom_y - top_y));
  bgMiddleColor = GColorMintGreen;
  timeTextColor = GColorBlack;
  text_layer_set_background_color(sTimeTextLayer, bgMiddleColor);
  text_layer_set_text_color(sTimeTextLayer, timeTextColor);
  text_layer_set_text(sTimeTextLayer, "00:00"); // Time text is set by updateTime, called by tick handler
  text_layer_set_font(sTimeTextLayer, sTimeFont);
  text_layer_set_text_alignment(sTimeTextLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(sTimeTextLayer));

  // now we can setup our weather and date text layers
  sWeatherTextLayer = text_layer_create(
    GRect(0, 2, windowBounds.size.w, top_y/2));
  weatherTextColor = GColorBlack;
  text_layer_set_background_color(sWeatherTextLayer, GColorClear);
  text_layer_set_text_color(sWeatherTextLayer, weatherTextColor);
  //text_layer_set_text(sWeatherTextLayer, "Weather");
  text_layer_set_font(sWeatherTextLayer, sTextFont);
  text_layer_set_text_alignment(sWeatherTextLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(sWeatherTextLayer));

  sTemperatureTextLayer = text_layer_create(
    GRect(0, (top_y/2) - 2, windowBounds.size.w, top_y/2));
  text_layer_set_background_color(sTemperatureTextLayer, GColorClear);
  text_layer_set_text_color(sTemperatureTextLayer, weatherTextColor);
  text_layer_set_text(sTemperatureTextLayer, "Temperature");
  text_layer_set_font(sTemperatureTextLayer, sTextFont);
  text_layer_set_text_alignment(sTemperatureTextLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(sTemperatureTextLayer));

  sDayTextLayer = text_layer_create(
    GRect(0, bottom_y+4, windowBounds.size.w, bottom_y/2));
  dateTextColor = GColorBlack;
  text_layer_set_background_color(sDayTextLayer, GColorClear);
  text_layer_set_text_color(sDayTextLayer, dateTextColor);
  text_layer_set_text(sDayTextLayer, "Date");
  text_layer_set_font(sDayTextLayer, sTextFont);
  text_layer_set_text_alignment(sDayTextLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(sDayTextLayer));

  sBattBtTextLayer = text_layer_create(
    GRect(0, (bottom_y) + (windowBounds.size.h - (bottom_y)) / 2, windowBounds.size.w, bottom_y/2));
  text_layer_set_background_color(sBattBtTextLayer, GColorClear);
  text_layer_set_text_color(sBattBtTextLayer, dateTextColor);
  text_layer_set_text(sBattBtTextLayer, "Battery");// NoBT");
  text_layer_set_font(sBattBtTextLayer, sTextFont);
  text_layer_set_text_alignment(sBattBtTextLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(sBattBtTextLayer));
}

// When the main window unloads
static void main_window_unload(Window *window)
{
  // Destroy TextLayer
  text_layer_destroy(sTimeTextLayer);
  layer_destroy(sBgTopLayer);
  layer_destroy(sBgBottomLayer);
  
  text_layer_destroy(sWeatherTextLayer);
  text_layer_destroy(sTemperatureTextLayer);
  text_layer_destroy(sDayTextLayer);
  text_layer_destroy(sBattBtTextLayer);

  // Destroy other assets
  fonts_unload_custom_font(sTimeFont);
  fonts_unload_custom_font(sTextFont);
}

// Tick handler called whenever the time changes
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  // The primary purpose of the tick handler is to update the time
  updateTime();

  // And now the weahter
  // Get weather update every 30 minutes
  if (tick_time->tm_min % 1 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void battery_handler(BatteryChargeState state)
{
  sBatteryLevel = state.charge_percent;
  // Update meter
  updateBatteryAndBT();
}

static void bt_handler(bool connected)
{
  sBTConnected = connected;
  updateBatteryAndBT();
}

// The below callbacks are part of the AppMessage system (how we get weather data from API)

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  static char temperatureBuffer[8], temperatureMinBuffer[8], temperatureMaxBuffer[8];
  static char conditionsBuffer[32], conditionsDescBuffer[32];
  static char weatherStringBuffer[32], temperatureStringBuffer[32]; // these strings will go into the text layers' texts

  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *tempMin_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATUREMIN);
  Tuple *tempMax_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATUREMAX);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);
  Tuple *conditionsDesc_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONSDESC);

  snprintf(temperatureBuffer, sizeof(temperatureBuffer), "%dC", (int)temp_tuple->value->int32);
  snprintf(conditionsBuffer, sizeof(conditionsBuffer), "%s", conditions_tuple->value->cstring);

  // Assemble full string and display
  snprintf(weatherStringBuffer, sizeof(weatherStringBuffer), "%s, %s", temperatureBuffer, conditionsBuffer);
  text_layer_set_text(sWeatherTextLayer, weatherStringBuffer);

  // If all data is available, use it
  if (temp_tuple && conditions_tuple) {

  }
  APP_LOG(APP_LOG_LEVEL_INFO, "The weather should have updated!");

}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  // Fun fact, this APP_LOG method can be used to print to the console as long as 
  // the install command has "--logs" included
  APP_LOG(APP_LOG_LEVEL_INFO, "My Outbox send success!");
}

// Face init code
static void init()
{
  // Create main Window element and assign to pointer
  sMainWindow = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(sMainWindow, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(sMainWindow, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Update the time now so we dont have to wait for the timer service
  updateTime();

  // Register with the battery charge service so we know the state of the battery
  battery_state_service_subscribe(battery_handler);
  // Ensure battery level is displayed from the start
  battery_handler(battery_state_service_peek());

  bluetooth_connection_service_subscribe(bt_handler);
  bt_handler(bluetooth_connection_service_peek());


  // Open AppMessage with some useful example buffer sizes
  const int inbox_size = 512;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);

  // Register callbacks to get weather data (and any other API online data)
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
}

// Face de-init code
static void deinit()
{
  // Destroy the main window on deinit
  window_destroy(sMainWindow);

  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  //app_message_deregister_callbacks();
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}