#include <pebble.h>

static Window *sMainWindow;

// When the main window first loads
static void main_window_load(Window *window)
{

}

// When the main window unloads
static void main_window_unload(Window *window)
{

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