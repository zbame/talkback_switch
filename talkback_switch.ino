#include "MIDIUSB.h"

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).


void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}


// ピンの設定
const int button = 2;
const int led = 3;

// A#5のノートナンバー
const int note = 94;

// ボタンの状態
char st;

void setup() {

  //ピンのモードを入力に設定
  pinMode(button, INPUT_PULLUP);

  //ピンのモードを出力に設定
  pinMode(led, OUTPUT);

  // ボタンの状態を初期化
  st = 0;
}

void loop() {

  // ボタンの監視
  switch(st) {
    case 0:
      if (digitalRead(button) == LOW) {
        // ボタンが押されたらノートオンメッセージを送信
        noteOn(0, note, 127);
        MidiUSB.flush();
        st = 1;
      }
      break;
    case 1:
      if (digitalRead(button) == HIGH) {
        // ボタンが離されたらノートオフメッセージを送信
        noteOff(0, note, 0);
        MidiUSB.flush();
        st = 0;  
      }
      break;
  }

  // LEDの制御
  midiEventPacket_t rx;
  rx = MidiUSB.read();
  if (rx.header != 0) {
    if (rx.byte2 == note) {
      if (rx.byte1 == 0x90) {
        if (rx.byte3 == 0) {
          // ノートオンメッセージをベロシティー０で受信したらLEDを消灯
          digitalWrite(led, LOW);
        } else {
          // ノートオンメッセージを受信したらLEDを点灯
          digitalWrite(led, HIGH);
        }
      } else if (rx.byte1 == 0x80) {
        // ノートオフメッセージを受信したらLEDを消灯
        digitalWrite(led, LOW);
      }
    }
  }
}
