// 5" Cheap yellow display
#ifndef _AUDIOPLAYER
#define _AUDIOPLAYER

#include "Arduino.h"
#include "FS.h"
#include "audioplayer.h"

// Digital I/O used
#define I2S_DOUT 17
#define I2S_BCLK 0
#define I2S_LRC 18

// Global audio object
Audio audio;

void setupAudio()
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); // 0...21
    Serial.println("Checking file");
    if (!SPIFFS.begin(true))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    Serial.println("Opening file");
    audio.connecttoFS(SPIFFS, "/Pink-Panther.wav");
}

// Service audio
void audioloop()
{
    audio.loop();
}

// Play single file
void play()
{
    Serial.println("Playing ");
    audio.connecttoFS(SPIFFS, "/Pink-Panther.wav");
}

#endif