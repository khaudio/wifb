#ifndef ESP32BUTTON_H
#define ESP32BUTTON_H

#include <chrono>

#ifdef ESP_PLATFORM
#include "driver/gpio.h"
#else
typedef int gpio_num_t;
#endif

/* Simple ESP32 input buttons */
namespace Esp32Button
{

class SimpleButton
{
protected:
    gpio_num_t _pin;
    bool _activeState, _state;

    virtual bool _read_input();

public:
    SimpleButton();
    SimpleButton(int pinNum, bool activeHigh = false);
    ~SimpleButton();

    void set_pin(int pinNum);
    void set_active_state(bool activeHigh);

    virtual bool read();
    virtual void set(bool state);
    virtual bool get() const;
    virtual void toggle();

    operator bool() const;
    bool operator!() const;
};

/* A soft, non-blocking debounce mechanism */
class Debouncer
{
protected:
    std::chrono::milliseconds _delay;
    
public:
    Debouncer();
    ~Debouncer();
    
    virtual void set_debounce_duration_ms(int ms);
    virtual bool _delay_met();
};

/* A button that must be held momentarily to change state */
class MomentaryButton : public SimpleButton
{
protected:
    std::chrono::milliseconds _holdDurationRequired;
    
    virtual bool _hold_duration_met();

public:
    MomentaryButton();
    MomentaryButton(int pinNum, bool activeHigh = false);
    ~MomentaryButton();
    
    virtual void set_hold_duration_ms(int ms);
    virtual bool read_hold();
};

/* Momentary button with an external override trigger */
class DualActionButton :
public MomentaryButton,
public Debouncer
{
protected:
    bool* _trigger;
    
public:
    DualActionButton();
    DualActionButton(int pinNum, bool activeHigh = false);
    DualActionButton(bool* target);
    ~DualActionButton();
    
    virtual void set_trigger(bool* target);
    virtual bool is_triggered();
    
    bool read() override;
    bool read_hold() override;
    virtual void set(bool state, bool force = false);
};

};

#endif
