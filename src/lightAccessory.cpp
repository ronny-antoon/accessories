#include "lightAccessory.hpp"

#include <on_error.hpp>
#include <iot_button.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

LightAccessory::LightAccessory(gpio_num_t button_pin, gpio_num_t led_pin)
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
    ESP_LOGV(__FILENAME__, "button_pin: %d, led_pin: %d", button_pin, led_pin);

    m_button_pin = button_pin;
    m_led_pin = led_pin;

    gpio_set_direction(m_button_pin, GPIO_MODE_INPUT);
    gpio_set_direction(m_led_pin, GPIO_MODE_INPUT_OUTPUT);

    gpio_set_level(m_led_pin, 0); // TODO: Change to last known state

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

void LightAccessory::setPower(bool power)
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
    ESP_LOGV(__FILENAME__, "power: %d", power);

    gpio_set_level(m_led_pin, power);
}

bool LightAccessory::getPower() const
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);
    ESP_LOGV(__FILENAME__, "return: %d", gpio_get_level(m_led_pin));

    return gpio_get_level(m_led_pin);
}

void LightAccessory::buttonCallback(void *button_handle, void *this_ptr)
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

    LightAccessory *lightAccessory = static_cast<LightAccessory *>(this_ptr);

    lightAccessory->setPower(!lightAccessory->getPower());
    lightAccessory->reportAttributes();
}

void LightAccessory::identifyYourSelf()
{
    ESP_LOGI(__FILENAME__, "call %s", __FUNCTION__);

    // run task for 3 secounds to blink the led
    xTaskCreate([](void *this_ptr)
                {
                    LightAccessory *lightAccessory = static_cast<LightAccessory *>(this_ptr);
                    lightAccessory->setPower(0);
                    for (int i = 0; i < 3; i++)
                    {
                        lightAccessory->setPower(1);
                        vTaskDelay(500 / portTICK_PERIOD_MS);
                        lightAccessory->setPower(0);
                        vTaskDelay(500 / portTICK_PERIOD_MS);
                    }
                    vTaskDelete(nullptr); },
                "identify_task", 2048, this, 5, nullptr);
}