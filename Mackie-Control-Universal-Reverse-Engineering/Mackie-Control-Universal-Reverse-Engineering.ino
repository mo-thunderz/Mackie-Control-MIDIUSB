#include <Control_Surface.h>

String trackNames[8];
String trackPan[8];
bool trackOnOff[8] = {true, true, true, true, true, true, true, true};
bool allTracksMute = false;
bool trackSolo[8] = {false, false, false, false, false, false, false, false};
bool trackRecRdy[8] = {false, false, false, false, false, false, false, false};
bool trackSelected[8] = {false, false, false, false, false, false, false, false};
int trackVol[8] = {0, 0, 0, 0, 0, 0, 0, 0};

bool play_song = false;
bool stop_song = true;
bool rec_song = false;
int beat_count = 0;
int measure_count_digit1 = 0;
int measure_count_digit2 = 0;
int measure_count_digit3 = 0;

// A MIDI over USB interface
// USBMIDI_Interface midi;
HardwareSerialMIDI_Interface midi = {Serial2, MIDI_BAUD};

bool channelMessageCallback(ChannelMessage cm) {
  
  if ((cm.header & 0xF0) == 0x90) { // if NOTE ON
    if (cm.header == 144 and cm.data1 > 15 and cm.data1 < 24)
      trackOnOff[cm.data1 - 16] = (bool) (127 - cm.data2);
    else if (cm.header == 144 and cm.data1 > 7 and cm.data1 < 16)
      trackSolo[cm.data1 - 8] = (bool) (cm.data2);
    else if (cm.header == 144 and cm.data1 == 115)
      allTracksMute = (bool) (cm.data2);
    else if (cm.header == 144 and cm.data1 < 9)
      trackRecRdy[cm.data1] = (bool) (cm.data2);
    else if (cm.header == 144 and cm.data1 > 23 and cm.data1 < 32)
      trackSelected[cm.data1 - 24] = (bool) (cm.data2);
    else if (cm.header == 144 and cm.data1 == 94)
      play_song = (bool) (cm.data2);
    else if (cm.header == 144 and cm.data1 == 93)
      stop_song = (bool) (cm.data2);
    else if (cm.header == 144 and cm.data1 == 95)
      rec_song = (bool) (cm.data2);
    else {
      Serial.println("channelMessageCallback - upper - cm.header = " + (String) cm.header + ", cm.data1 = " + (String) cm.data1 + ", cm.data2 = " + (String) cm.data2);
      Serial << hex << cm.header << ' ' << cm.data1 << ' ' << cm.data2 << dec 
             << "\t(" <<  MCU::getMCUNameFromNoteNumber(cm.data1)  << ")" << endl;
    }
  }
  else 
    if (cm.header > 223 and cm.header < 232 ) {
      trackVol[cm.header - 224] = ((unsigned int)cm.data2 << 7) + cm.data1;
      // Serial.println("Volume Track " + String(cm.header - 224) + ": " + trackVol[cm.header - 224]);
    }
    else if (cm.header == 176 and cm.data1 > 63 and cm.data1 < 66 ) {   // cm.header == 176 -> CC message
      // triggers for 16th and 32nd notes
    }
    else if (cm.header == 176 and cm.data1 == 67) {
      // trigger for 8th notes -> combine with beat -> subtract 114
    }
    else if (cm.header == 176 and cm.data1 == 69) {
      beat_count = cm.data2 - 113;
      Serial.println("beat: " + String(beat_count));
    }
    else if (cm.header == 176 and cm.data1 == 73) {
      measure_count_digit3 = cm.data2 - 48;
      // Serial.println("measure: " + String(100*measure_count_digit3 + 10*measure_count_digit2 + measure_count_digit1));
    }
    else if (cm.header == 176 and cm.data1 == 72) {
      measure_count_digit2 = cm.data2 - 48;
      // Serial.println("measure: " + String(100*measure_count_digit3 + 10*measure_count_digit2 + measure_count_digit1));
    }
    else if (cm.header == 176 and cm.data1 == 71) {
      measure_count_digit1 = cm.data2 - 113;
      Serial.println("measure: " + String(100*measure_count_digit3 + 10*measure_count_digit2 + measure_count_digit1));
    }
    else {
      Serial << hex << cm.header << ' ' << cm.data1 << ' ' << cm.data2 << dec; // << endl;
      Serial.println(" - Lower - cm.header = " + (String) cm.header + ", cm.data1 = " + (String) cm.data1 + ", cm.data2 = " + (String) cm.data2);      
    }
  return false; // Return true to indicate that handling is done,
                // and Control_Surface shouldn't handle it anymore.
                // If you want Control_Surface to handle it as well,
                // return false.
}
 
bool sysExMessageCallback(SysExMessage se) {
  bool recognized = false;
  
  if(se.length == 64) {
    if(se.data[0] == 240 and se.data[6] == 0) {         // track names
      recognized = true;
      for (int i = 0; i < 8; ++i)
        trackNames[i] = "";
      for (size_t i = 0; i < se.length; ++i) {
        if(i > 6 and i < 13)
          trackNames[0].concat((char)se.data[i]);
        if(i > 13 and i < 20)
          trackNames[1].concat((char)se.data[i]);
        if(i > 20 and i < 27)
          trackNames[2].concat((char)se.data[i]);
        if(i > 27 and i < 34)
          trackNames[3].concat((char)se.data[i]);
        if(i > 34 and i < 41)
          trackNames[4].concat((char)se.data[i]);
        if(i > 41 and i < 48)
          trackNames[5].concat((char)se.data[i]);
        if(i > 48 and i < 55)
          trackNames[6].concat((char)se.data[i]);
        if(i > 55 and i < 62)
          trackNames[7].concat((char)se.data[i]);    
      }
    }
    if(se.data[0] == 240 and se.data[6] == 56) {      // panning
      recognized = true;
      for (int i = 0; i < 8; ++i)
        trackPan[i] = "";
      for (size_t i = 0; i < se.length; ++i) {
        if(i > 6 and i < 13)
          trackPan[0].concat((char)se.data[i]);
        if(i > 13 and i < 20)
          trackPan[1].concat((char)se.data[i]);
        if(i > 20 and i < 27)
          trackPan[2].concat((char)se.data[i]);
        if(i > 27 and i < 34)
          trackPan[3].concat((char)se.data[i]);
        if(i > 34 and i < 41)
          trackPan[4].concat((char)se.data[i]);
        if(i > 41 and i < 48)
          trackPan[5].concat((char)se.data[i]);
        if(i > 48 and i < 55)
          trackPan[6].concat((char)se.data[i]);
        if(i > 55 and i < 62)
          trackPan[7].concat((char)se.data[i]);    
      }
    }
    for (int i = 0; i < 8; ++i) {
      Serial.println(trackNames[i] + ", pan: " + trackPan[i] + ", on/off: " + trackOnOff[i] + ", solo: " + trackSolo[i] + ", rec: " + trackRecRdy[i] + ", sel: " + trackSelected[i] + ", vol: " + trackVol[i]);
    }
  }
  if(recognized == false) {
    Serial << F("sysExMessageCallback with length: ") << se.length << F(", ");
    for (size_t i = 0; i < se.length; ++i) {
      Serial << se.data[i] << ' ';  
    }
    Serial << dec << F("on cable ") << se.CN << endl;
  }
  
  return false; // Return true to indicate that handling is done,
                // and Control_Surface shouldn't handle it anymore.
                // If you want Control_Surface to handle it as well,
                // return false.
}
 
bool realTimeMessageCallback(RealTimeMessage rt) {
  Serial << F("Real-Time: ") << hex << rt.message << dec
         << F(" on cable ") << rt.CN << endl;
  return false; // Return true to indicate that handling is done,
                // and Control_Surface shouldn't handle it anymore.
                // If you want Control_Surface to handle it as well,
                // return false.
}
 
void setup() {
  // Make sure that the Serial interface is fast enough, so it doesn't stall the MIDI 
  // application
  Serial.begin(1000000);
  Control_Surface.begin();
  Control_Surface.setMIDIInputCallbacks(channelMessageCallback,   //
                                        sysExMessageCallback,     //
                                        realTimeMessageCallback); //
}
 
void loop() {
  Control_Surface.loop();
}
