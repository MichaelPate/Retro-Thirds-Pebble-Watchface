#include <pebble.h>

// Window Objects
static Window *sMainWindow;

// Layer Objects, each line of text has its own layer
static TextLayer *sWeatherTextLayer, *sTemperatureTextLayer, *sTodayTemperatureTextLayer;
static TextLayer *sTimeTextLayer;
static TextLayer *sDayTextLayer, *sDateTextLayer, *sBattBTTextLayer;

// Our background color layers
static Layer *sBgTopLayer, *sBgMiddleLayer, *sBgBottomLayer;
const int top_y = 20, bottom_y = 40; // which y level will the layers be defined at

// When the main window first loads
static void main_window_load(Window *window)
{
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  sTimeTextLayer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(sTimeTextLayer, GColorClear);
  text_layer_set_text_color(sTimeTextLayer, GColorBlack);
  text_layer_set_text(sTimeTextLayer, "00:00");
  text_layer_set_font(sTimeTextLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(sTimeTextLayer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(sTimeTextLayer));
}

// When the main window unloads
static void main_window_unload(Window *window)
{
  // Destroy TextLayer
  text_layer_destroy(sTimeTextLayer);
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