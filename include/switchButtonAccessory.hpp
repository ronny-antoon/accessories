#ifndef SWITCH_BUTTON_ACCESSORY_HPP
#define SWITCH_BUTTON_ACCESSORY_HPP

#include <cstdint>
#include <hal/gpio_types.h>

#include "accessory_interface.hpp"

/**
 * @brief A switch button accessory.
 *
 * This class defines a switch button accessory.
 */
class SwitchButtonAccessory : public AccessoryInterface
{
public:
    enum PressType : uint8_t
    {
        SinglePress = 1, ///< Represents a single press event.
        DoublePress = 2, ///< Represents a double press event.
        LongPress = 3    ///< Represents a long press event.
    };

    /**
     * @brief Construct a new Switch Button Accessory object.
     *
     * @param button_pin The GPIO pin number of the button.
     *
     * @note The button is assumed to be connected to the GPIO pin in pull-up mode.
     */
    SwitchButtonAccessory(gpio_num_t button_pin);

    /**
     * @brief Destroy the Switch Button Accessory object.
     */
    ~SwitchButtonAccessory() = default;

    /**
     * @brief Identify the accessory.
     */
    void identifyYourSelf() override;

    /**
     * @brief Get the last press type.
     *
     * @return The last press type.
     */
    PressType getLastPressType() const;

private:
    gpio_num_t m_button_pin;     ///< The GPIO pin number of the button.
    PressType m_last_press_type; ///< The last press type.
};

#endif // SWITCH_BUTTON_ACCESSORY_HPP