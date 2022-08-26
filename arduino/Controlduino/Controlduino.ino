/*
  Controlduino
*/

#include <Arduino.h>
#include <avr/io.h>

#include "nano.h"
#include "arduino_xinput.h"

void initTimer();

Ard_XInput state;
const uint8_t* buff = (uint8_t*)&state;
constexpr uint8_t buff_size = sizeof(state);

void setup() {
  // Initialize pins
  pinMode(pins::LeftStick_X, INPUT);
  pinMode(pins::LeftStick_Y, INPUT);
  pinMode(pins::LeftStick_Btn, INPUT);
  pinMode(pins::RightStick_X, INPUT);
  pinMode(pins::RightStick_Y, INPUT);
  pinMode(pins::RightStick_Btn, INPUT);
  pinMode(pins::DPad_Up, INPUT);
  pinMode(pins::DPad_Right, INPUT);
  pinMode(pins::DPad_Down, INPUT);
  pinMode(pins::DPad_Left, INPUT);
  pinMode(pins::Button_Y, INPUT);
  pinMode(pins::Button_B, INPUT);
  pinMode(pins::Button_A, INPUT);
  pinMode(pins::Button_X, INPUT);
  pinMode(pins::BumperL, INPUT);
  pinMode(pins::BumperR, INPUT);
  pinMode(pins::TriggerL, INPUT);
  pinMode(pins::TriggerR, INPUT);
  pinMode(pins::Start, INPUT);
  pinMode(pins::Back, INPUT);

  Serial.begin(9600);

  state.buttons = 0;
  state.lStickX = 0;
  state.lStickY = 0;
  state.lTrigger = 0;
  state.rTrigger = 0;

  initTimer();
}

// Automatically called by timer 1
ISR(TIMER1_COMPA_vect)
{
  state.buttons = 0;
  
  if (digitalRead(pins::DPad_Up)) state.buttons |= button::Up;
  if (digitalRead(pins::DPad_Down)) state.buttons |= button::Down;
  if (digitalRead(pins::DPad_Left)) state.buttons |= button::Left;
  if (digitalRead(pins::DPad_Right)) state.buttons |= button::Right;

  if (digitalRead(pins::Button_A)) state.buttons |= button::A;
  if (digitalRead(pins::Button_B)) state.buttons |= button::B;
  if (digitalRead(pins::Button_X)) state.buttons |= button::X;
  if (digitalRead(pins::Button_Y)) state.buttons |= button::Y;

  if (digitalRead(pins::BumperL)) state.buttons |= button::LB;
  if (digitalRead(pins::BumperR)) state.buttons |= button::RB;

  if (digitalRead(pins::Start)) state.buttons |= button::Start;
  if (digitalRead(pins::Back)) state.buttons |= button::Back;

  if (digitalRead(pins::LeftStick_Btn)) state.buttons |= button::L3;
  if (digitalRead(pins::RightStick_Btn)) state.buttons |= button::R3;

  if (digitalRead(pins::TriggerL))
  {
    state.lTrigger = 255;
  }
  else
  {
    state.lTrigger = 0;
  }

  if (digitalRead(pins::TriggerR))
  {
    state.rTrigger = 255;
  }
  else
  {
    state.rTrigger = 0;
  }

  state.lStickX = analogRead(pins::LeftStick_X);
  state.lStickY = analogRead(pins::LeftStick_Y);

  state.rStickX = analogRead(pins::RightStick_X);
  state.rStickY = analogRead(pins::RightStick_Y);

  Serial.write(buff, buff_size);
}

void initTimer()
{
  // Disable global interrupts
  cli();
  // Reset Timer counter control registers
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer output compare register to freq
  // TODO finetune this value
  OCR1A = 10000;

  // Turn on CTC (clear timer on compare match)
  TCCR1B |= (1 << WGM12);
  // Prescalar = 64
  TCCR1B |= 0b00000101;

  // Enable timer interrupt
  TIMSK1 |= (1 << OCIE1A);

  // Enable interrupts
  sei();
}

void loop() 
{
  // None
}