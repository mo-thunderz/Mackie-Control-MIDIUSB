// Simple program to read track names from DAW using the MackieControl protocol
// In Arduino IDE navigate to "Sketch" -> "Include Library" -> "Manage Libraries"
// Search for "USB-MIDI" and install the "USB-MIDI" program by lathoub (I tested version 1.1.2)
//
// Tested on Arduino Due
// connect "Programming Port" to computer and open serial monitor
// connect "Native USB Port" to (same) computer for USB-based MIDI interface
//
// Setup DAW
// Open your DAW, go to settings and configure a MackieControl interface using the USB-MIDI interface for OUTPUT
//
// The names of the tracks should now appear in the serial monitor of the Arduino Due. Every time you add or delete a track the names will be refreshed.

#include <USB-MIDI.h>       // required for Midi
String trackNames[8];       // variable where track names are stored
const byte *  se;           // variable for Sysex

USBMIDI_CREATE_DEFAULT_INSTANCE();

unsigned long t0 = millis();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial);

  // Listen for MIDI messages on channel 1
  MIDI.begin(1);

  MIDI.setHandleSystemExclusive(OnMidiSysEx);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop()
{
  // Listen to incoming notes
  MIDI.read();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnMidiSysEx(byte* data, unsigned length) {
  
  if (length == 64) {
    if(data[0] == 240 and data[6] == 0) {         // Test if bit 6 is 0 to make sure track names are sent
      for (int i = 0; i < 8; ++i)
        trackNames[i] = "";
      for (size_t i = 0; i < 64; ++i) {
        if(i > 6 and i < 13)
          trackNames[0].concat((char)data[i]);
        if(i > 13 and i < 20)
          trackNames[1].concat((char)data[i]);
        if(i > 20 and i < 27)
          trackNames[2].concat((char)data[i]);
        if(i > 27 and i < 34)
          trackNames[3].concat((char)data[i]);
        if(i > 34 and i < 41)
          trackNames[4].concat((char)data[i]);
        if(i > 41 and i < 48)
          trackNames[5].concat((char)data[i]);
        if(i > 48 and i < 55)
          trackNames[6].concat((char)data[i]);
        if(i > 55 and i < 62)
          trackNames[7].concat((char)data[i]);    
      }
      Serial.println("Track Names: ");
      for (int i = 0; i < 8; ++i)
        Serial.println(trackNames[i]);
    }
  }
}



//-------------------------------------------------------------------------------------//
// *** MIDI Information ***
//
//  NoteOff               = 0x80, ///< Note Off
//  NoteOn                = 0x90, ///< Note On
//  AfterTouchPoly        = 0xA0, ///< Polyphonic AfterTouch
//  ControlChange         = 0xB0, ///< Control Change / Channel Mode
//  ProgramChange         = 0xC0, ///< Program Change
//  AfterTouchChannel     = 0xD0, ///< Channel (monophonic) AfterTouch
//  PitchBend             = 0xE0, ///< Pitch Bend
//  SystemExclusive       = 0xF0, ///< System Exclusive
//  TimeCodeQuarterFrame  = 0xF1, ///< System Common - MIDI Time Code Quarter Frame
//  SongPosition          = 0xF2, ///< System Common - Song Position Pointer
//  SongSelect            = 0xF3, ///< System Common - Song Select
//  TuneRequest           = 0xF6, ///< System Common - Tune Request
//  Clock                 = 0xF8, ///< System Real Time - Timing Clock
//  Start                 = 0xFA, ///< System Real Time - Start
//  Continue              = 0xFB, ///< System Real Time - Continue
//  Stop                  = 0xFC, ///< System Real Time - Stop
//  ActiveSensing         = 0xFE, ///< System Real Time - Active Sensing
//  SystemReset           = 0xFF, ///< System Real Time - System Reset
//  InvalidType           = 0x00    ///< For notifying errors
