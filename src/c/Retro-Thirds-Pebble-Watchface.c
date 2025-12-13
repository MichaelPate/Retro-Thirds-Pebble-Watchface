#include <pebble.h>

// Window Objects
static Window *sMainWindow;

// Layer Objects, each line of text has its own layer
static TextLayer *sWeatherTextLayer, *sTemperatureTextLayer, *sTodayTemperatureTextLayer;
static TextLayer *sTimeTextLayer;
static TextLayer *sDayTextLayer, *sDateTextLayer, *sBattBTTextLayer;

static GFont sTimeFont, sTextFont;

// Our background color layers
static Layer *sBgTopLayer, *sBgBottomLayer; // no background for middle layer since time text just uses its BG color
#define top_y 54//PBL_IF_ROUND_ELSE(58, 52); // Need to go back
//const int middle_size = PBL_IF_ROUND_ELSE(58, 52);
#define bottom_y top_y + 58 // which y level will the layers be defined at
static GColor bgTopColor, bgMiddleColor, bgBottomColor;
static GColor weatherTextColor, timeTextColor, dateTextColor;

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

// Normally this would go in the tick handler directly,
// but its its own function so we can also call it whenever we want (init)
static void updateTime()
{
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(sTimeTextLayer, s_buffer);
}

// When the main window first loads
static void main_window_load(Window *window)
{
  // Get information about the Window for the watchface
  Layer *window_layer = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(window_layer);

  // Initialize fonts
  sTimeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_ABDUCTION_48));

  // Create the time TextLayer with specific bounds (in its third of the screen) and colors
  sTimeTextLayer = text_layer_create(
      GRect(0, top_y, windowBounds.size.w, bottom_y - top_y));
  bgMiddleColor = GColorGreen;
  timeTextColor = GColorBlack;
  text_layer_set_background_color(sTimeTextLayer, bgMiddleColor);
  text_layer_set_text_color(sTimeTextLayer, timeTextColor);
  //text_layer_set_text(sTimeTextLayer, "00:00"); // Time text is set by updateTime, called by tick handler
  text_layer_set_font(sTimeTextLayer, sTimeFont);
  //text_layer_set_font(sTimeTextLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(sTimeTextLayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(sTimeTextLayer));

  // Create our background color layers and add them as well
  sBgTopLayer = layer_create(GRect(0, 0, windowBounds.size.w, top_y));
  bgTopColor = GColorBlue;
  layer_set_update_proc(sBgTopLayer, topThirdBgColor_proc);
  layer_add_child(window_layer, sBgTopLayer);

  sBgBottomLayer = layer_create(GRect(0, bottom_y, windowBounds.size.w, windowBounds.size.h));
  bgBottomColor = GColorRed;
  layer_set_update_proc(sBgBottomLayer, bottomThirdBgColor_proc);
  layer_add_child(window_layer, sBgBottomLayer);

  // now we can setup our weather and date text layers
}

// When the main window unloads
static void main_window_unload(Window *window)
{
  // Destroy TextLayer
  text_layer_destroy(sTimeTextLayer);
  layer_destroy(sBgTopLayer);
  layer_destroy(sBgBottomLayer);

  // Destroy other assets
  fonts_unload_custom_font(sTimeFont);
}

// Tick handler called whenever the time changes
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  // The primary purpose of the tick handler is to update the time
  updateTime();
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
}

// Face de-init code
static void deinit()
{
  // Destroy the main window on deinit
  window_destroy(sMainWindow);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}