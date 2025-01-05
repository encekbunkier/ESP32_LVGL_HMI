#include "Arduino.h"
#include "lvgl_ui.h"
#include "audioplayer.h"

void setup() 
{
  setupUI();
  showMainScreen();

  // Configure I2S
  //setupAudio();
}

void loop()
{
  // Update UI
  uiLoop();

  // Service audio
  //audioloop();
}
