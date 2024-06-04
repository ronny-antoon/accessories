#ifndef DOOR_ACCESSORY_HPP
#define DOOR_ACCESSORY_HPP

#include <cstdint>
#include <hal/gpio_types.h>

#include "accessory_interface.hpp"

/**
 * @brief A door accessory.
 *
 * This class defines a door accessory.
 */
class DoorAccessory : public AccessoryInterface
{
public:
    /**
     * @brief Construct a new Door Accessory object.
     *
     * @param button_pin The GPIO pin number of the button.
     * @param door_pin The GPIO pin number of the LED.
     * @param open_time The time in seconds the door will stay open.
     *
     * @note The button is assumed to be connected to the GPIO pin in pull-up mode.
     */
    DoorAccessory(gpio_num_t button_pin, gpio_num_t door_pin, uint32_t open_time);

    /**
     * @brief Destroy the Door Accessory object.
     */
    ~DoorAccessory() = default;

    /**
     * @brief Open the door.
     */
    void openDoor();

    /**
     * @brief Check if the door is open.
     *
     * @return true if the door is open, false otherwise.
     */
    bool isDoorOpen() const;

    /**
     * @brief Identify the accessory.
     */
    void identifyYourSelf() override;

private:
    gpio_num_t m_button_pin; ///< The GPIO pin number of the button.
    gpio_num_t m_door_pin;   ///< The GPIO pin number of the LED.
    uint32_t m_open_time;    ///< The time in seconds the door will stay open.

    /**
     * @brief The callback function for the button.
     *
     * @param button_handle The handle of the button.
     * @param this_ptr The pointer to the DoorAccessory object.
     */
    static void buttonCallback(void *button_handle, void *this_ptr);
};

#endif // DOOR_ACCESSORY_HPP