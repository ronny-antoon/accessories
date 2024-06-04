#include "doorAccessory.hpp"

#include <on_error.hpp>
#include <iot_button.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

DoorAccessory::DoorAccessory(gpio_num_t button_pin, gpio_num_t door_pin, uint32_t open_time)
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
    ESP_LOGV(__FILENAME__, "button_pin: %d, door_pin: %d, open_time: %d", button_pin, door_pin, (int)open_time);

    m_button_pin = button_pin;
    m_door_pin = door_pin;
    m_open_time = open_time;

    gpio_set_direction(m_button_pin, GPIO_MODE_INPUT);
    gpio_set_direction(m_door_pin, GPIO_MODE_INPUT_OUTPUT);

    gpio_set_level(m_door_pin, 0);

    button_config_t button_config = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = m_button_pin,
            .active_level = 1,
        }};

    button_handle_t handle = iot_button_create(&button_config);

    // buttonCallback
    iot_button_register_cb(handle, BUTTON_PRESS_DOWN, buttonCallback, this);
}

void DoorAccessory::openDoor()
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

    xTaskCreate([](void *this_ptr)
                {
        DoorAccessory *doorAccessory = static_cast<DoorAccessory *>(this_ptr);

        gpio_set_level(doorAccessory->m_door_pin, 1);
        vTaskDelay(doorAccessory->m_open_time * 1000 / portTICK_PERIOD_MS);
        gpio_set_level(doorAccessory->m_door_pin, 0);
        doorAccessory->reportAttributes(); }, // Report the new state of the door
                "openDoorTask",
                4096, this, 5, nullptr);
}

void DoorAccessory::buttonCallback(void *button_handle, void *this_ptr)
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

    DoorAccessory *doorAccessory = static_cast<DoorAccessory *>(this_ptr);

    doorAccessory->openDoor();
    doorAccessory->reportAttributes();
}

void DoorAccessory::identifyYourSelf()
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

    openDoor();
}

bool DoorAccessory::isDoorOpen() const
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

    return gpio_get_level(m_door_pin);
}