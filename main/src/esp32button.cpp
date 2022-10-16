#include "Esp32Button.h"

using namespace Esp32Button;

SimpleButton::SimpleButton() :
_activeState(false),
_state(false)
{
}

SimpleButton::SimpleButton(int pinNum, bool activeHigh) :
SimpleButton()
{
    this->_pin = static_cast<gpio_num_t>(pinNum);
    this->_activeState = activeHigh;
    
    #ifdef ESP_PLATFORM
    gpio_config_t inputConf;
    inputConf.pin_bit_mask = (1ULL << this->_pin);
    inputConf.mode = GPIO_MODE_INPUT;
    inputConf.intr_type = static_cast<gpio_int_type_t>(GPIO_INTR_DISABLE);
    if (this->_activeState)
    {
        inputConf.pull_up_en = GPIO_PULLUP_DISABLE;
        inputConf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    }
    else
    {
        inputConf.pull_up_en = GPIO_PULLUP_ENABLE;
        inputConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    }
    gpio_config(&inputConf);
    #endif

    read();
}

SimpleButton::~SimpleButton()
{
}

void SimpleButton::set_pin(int pinNum)
{
    this->_pin = static_cast<gpio_num_t>(pinNum);
}

void SimpleButton::set_active_state(bool activeHigh)
{
    this->_activeState = activeHigh;
}

inline bool SimpleButton::_read_input()
{
    /* Return GPIO input */
    #ifdef ESP_PLATFORM
    return (this->_activeState == gpio_get_level(this->_pin));
    #else
    return this->_state;
    #endif
}

inline bool SimpleButton::read()
{
    /* Read and set state */
    set(_read_input());
    return this->_state;
}

inline void SimpleButton::set(bool state)
{
    this->_state = state;
}

inline bool SimpleButton::get() const
{
    return this->_state;
}

inline void SimpleButton::toggle()
{
    /* Manually toggle state,
    rather than reading input pin */
    set(!this->_state);
}

SimpleButton::operator bool() const
{
    return this->_state;
}

bool SimpleButton::operator!() const
{
    return !(this->_state);
}

Debouncer::Debouncer() :
_delay(5)
{
}

Debouncer::~Debouncer()
{
}

void Debouncer::set_debounce_duration_ms(int ms)
{
    this->_delay = std::chrono::milliseconds(ms);
}

bool Debouncer::_delay_met()
{
    static std::chrono::high_resolution_clock::time_point timeNow;
    static std::chrono::high_resolution_clock::time_point stateLastSet = (
            std::chrono::high_resolution_clock::now() - this->_delay
        );
    if (this->_delay <= std::chrono::milliseconds(0)) return true;
    timeNow = std::chrono::high_resolution_clock::now();
    if ((timeNow - stateLastSet) >= this->_delay)
    {
        stateLastSet = timeNow;
        return true;
    }
    return false;
}

MomentaryButton::MomentaryButton() :
SimpleButton(),
_holdDurationRequired(1)
{
}

MomentaryButton::MomentaryButton(int pinNum, bool activeHigh) :
SimpleButton(pinNum, activeHigh)
{
}

MomentaryButton::~MomentaryButton()
{
}

void MomentaryButton::set_hold_duration_ms(int ms)
{
    this->_holdDurationRequired = std::chrono::milliseconds(ms); 
}

bool MomentaryButton::_hold_duration_met()
{
    /* Measure hold duration
    Returns true only when duration is met
    Continuing to hold will return false */
    using namespace std::chrono;
    static bool reading(false);
    static high_resolution_clock::time_point start = high_resolution_clock::now();
    static high_resolution_clock::time_point timeNow;
    timeNow = high_resolution_clock::now();
    if (reading && ((timeNow - start) >= this->_holdDurationRequired))
    {
        this->toggle();
        reading = false;
        return true;
    }
    start = timeNow;
    reading = true;
    return false;
}

bool MomentaryButton::read_hold()
{
    /* Returns true when button has been
    held for specified duration */
    bool inputIsActive = _read_input();
    if (!inputIsActive)
    {
        set(false);
    }
    else if (!this->_state)
    {
        return this->_hold_duration_met();
    }
    return false;
}

DualActionButton::DualActionButton() :
MomentaryButton(),
Debouncer()
{
    this->_trigger = nullptr;
    
    /* Default to inactive soft debounce */
    set_debounce_duration_ms(0);
}

DualActionButton::DualActionButton(int pinNum, bool activeHigh) :
MomentaryButton(pinNum, activeHigh)
{
}

DualActionButton::DualActionButton(bool* target) :
MomentaryButton(),
_trigger(target)
{
}

DualActionButton::~DualActionButton()
{
    this->_trigger = nullptr;
}

void DualActionButton::set_trigger(bool* target)
{
    this->_trigger = target;
}

inline bool DualActionButton::is_triggered()
{
    return ((this->_trigger != nullptr) && *this->_trigger);
}

inline bool DualActionButton::read()
{
    set(SimpleButton::_read_input() || is_triggered());
    return this->_state;
}

bool DualActionButton::read_hold() 
{
    /* Returns true when button has been
    either held or triggered externally
    for specified duration */
    bool inputIsActive = _read_input();
    if (this->_state && !inputIsActive)
    {
        set(false);
    }
    else if ((this->_state != inputIsActive) || is_triggered())
    {
        return MomentaryButton::_hold_duration_met();
    }
    return false;
}

inline void DualActionButton::set(bool state, bool force)
{
    if (Debouncer::_delay_met() || force) SimpleButton::set(state);
}