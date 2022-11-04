/*
  Controlduino
*/

#include <Arduino.h>
#include <avr/io.h>

#include "arduino_xinput.h"
#include "nano.h"

void initTimer();

Ard_XInput state;
const uint8_t* buff = (uint8_t*)&state;
constexpr uint8_t buff_size = sizeof(state);

bool running = false;

constexpr unsigned long RATE = 1;

unsigned long lastTime;

void setup()
{
    // Initialize pins
    pinMode(pins::LeftStick_X, INPUT);
    pinMode(pins::LeftStick_Y, INPUT);
    pinMode(pins::LeftStick_Btn, INPUT_PULLUP);
    pinMode(pins::RightStick_X, INPUT);
    pinMode(pins::RightStick_Y, INPUT);
    pinMode(pins::RightStick_Btn, INPUT_PULLUP);
    pinMode(pins::DPad_Up, INPUT_PULLUP);
    pinMode(pins::DPad_Right, INPUT_PULLUP);
    pinMode(pins::DPad_Down, INPUT_PULLUP);
    pinMode(pins::DPad_Left, INPUT_PULLUP);
    pinMode(pins::Button_Y, INPUT_PULLUP);
    pinMode(pins::Button_B, INPUT_PULLUP);
    pinMode(pins::Button_A, INPUT_PULLUP);
    pinMode(pins::Button_X, INPUT_PULLUP);
    pinMode(pins::BumperL, INPUT_PULLUP);
    pinMode(pins::BumperR, INPUT_PULLUP);
    pinMode(pins::TriggerL, INPUT_PULLUP);
    pinMode(pins::TriggerR, INPUT_PULLUP);
    pinMode(pins::Start, INPUT_PULLUP);
    //pinMode(pins::Back, INPUT_PULLUP);
    pinMode(13, OUTPUT);

    Serial.begin(9600, SERIAL_8N1);

    state.buttons = 0;
    state.lStickX = 0;
    state.lStickY = 0;
    state.lTrigger = 0;
    state.rTrigger = 0;

    //initTimer();
    lastTime = millis();
}

bool light = false;

void loop()
{
    if(Serial.available())
    {
        uint8_t data;
        digitalWrite(13, HIGH);
        Serial.readBytes(&data, 1);
        Serial.write(data + 1);
    }

    return;

    unsigned long time = millis();

    unsigned long diff = time - lastTime;

    if(diff < RATE)
    {
        return;
    }

    lastTime = time;

    if(Serial.available() > 0)
    {
        uint8_t data;
        Serial.readBytes(&data, 1);
        if(data == START_RUN && !running)
        {
            running = true;
            digitalWrite(13, HIGH);
        }
        else if(data == STOP_RUN && running)
        {
            running = false;
            digitalWrite(13, LOW);
        }
    }

    if(!running)
    {
        return;
    }

    state.buttons = 0;

    if(!digitalRead(pins::DPad_Up))
        state.buttons |= button::Up;
    if(!digitalRead(pins::DPad_Down))
        state.buttons |= button::Down;
    if(!digitalRead(pins::DPad_Left))
        state.buttons |= button::Left;
    if(!digitalRead(pins::DPad_Right))
        state.buttons |= button::Right;

    if(!digitalRead(pins::Button_A))
        state.buttons |= button::A;
    if(!digitalRead(pins::Button_B))
        state.buttons |= button::B;
    if(!digitalRead(pins::Button_X))
        state.buttons |= button::X;
    if(!digitalRead(pins::Button_Y))
        state.buttons |= button::Y;

    if(!digitalRead(pins::BumperL))
        state.buttons |= button::LB;
    if(!digitalRead(pins::BumperR))
        state.buttons |= button::RB;

    if(!digitalRead(pins::Start))
        state.buttons |= button::Start;
    /* todo move to a define block
        depends on if the board has enough pins
    if(!digitalRead(pins::Back))
        state.buttons |= button::Back;
    */
    if(!digitalRead(pins::LeftStick_Btn))
        state.buttons |= button::L3;
    if(!digitalRead(pins::RightStick_Btn))
        state.buttons |= button::R3;

    state.lTrigger = digitalRead(pins::TriggerL) ? 0 : 255;
    state.rTrigger = digitalRead(pins::TriggerR) ? 0 : 255;

    state.lStickX = analogRead(pins::LeftStick_X);
    state.lStickY = analogRead(pins::LeftStick_Y);

    state.rStickX = analogRead(pins::RightStick_X);
    state.rStickY = analogRead(pins::RightStick_Y);

    Serial.write(buff, buff_size);
}

// Automatically called by timer 1
ISR(TIMER1_COMPA_vect)
{
    /*
    if(!running)
    {
        return;
    }

    state.buttons = 0;

    if(!digitalRead(pins::DPad_Up))
        state.buttons |= button::Up;
    if(!digitalRead(pins::DPad_Down))
        state.buttons |= button::Down;
    if(!digitalRead(pins::DPad_Left))
        state.buttons |= button::Left;
    if(!digitalRead(pins::DPad_Right))
        state.buttons |= button::Right;

    if(!digitalRead(pins::Button_A))
        state.buttons |= button::A;
    if(!digitalRead(pins::Button_B))
        state.buttons |= button::B;
    if(!digitalRead(pins::Button_X))
        state.buttons |= button::X;
    if(!digitalRead(pins::Button_Y))
        state.buttons |= button::Y;

    if(!digitalRead(pins::BumperL))
        state.buttons |= button::LB;
    if(!digitalRead(pins::BumperR))
        state.buttons |= button::RB;

    if(!digitalRead(pins::Start))
        state.buttons |= button::Start;
    */
    /* todo move to a define block
        depends on if the board has enough pins
    if(!digitalRead(pins::Back))
        state.buttons |= button::Back;
    */
    /*
     if(!digitalRead(pins::LeftStick_Btn))
         state.buttons |= button::L3;
     if(!digitalRead(pins::RightStick_Btn))
         state.buttons |= button::R3;

     state.lTrigger = digitalRead(pins::TriggerL) ? 0 : 255;
     state.rTrigger = digitalRead(pins::TriggerR) ? 0 : 255;

     state.lStickX = analogRead(pins::LeftStick_X);
     state.lStickY = analogRead(pins::LeftStick_Y);

     state.rStickX = analogRead(pins::RightStick_X);
     state.rStickY = analogRead(pins::RightStick_Y);

     Serial.write(buff, buff_size);
     */
}

/*
void initTimer()
{
    // Disable global interrupts
    cli();
    // Reset Timer counter control registers
    TCCR1A = 0;
    TCCR1B = 0;

    // Set timer output compare register to freq
    // TODO finetune this value
    //OCR1A = 5000;
    OCR1A = 100000;

    // Turn on CTC (clear timer on compare match)
    TCCR1B |= (1 << WGM12);
    // Prescalar
    // 1 (none) = 001
    // 8 = 010
    // 64 = 011
    // 256 = 100
    // 1024 = 101
    TCCR1B |= 0b00000011;

    // Enable timer interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Enable interrupts
    sei();
}
*/
