#pragma once
#include <cstdint>
#include <driver/i2c_master.h>
#include <array>

class I2C
{
private:
    static constexpr ::i2c_port_t I2C_PORT = i2c_port_t::I2C_NUM_0;
    static constexpr ::gpio_num_t I2C_SCL_PIN = gpio_num_t::GPIO_NUM_7;
    static constexpr ::gpio_num_t I2C_SDA_PIN = gpio_num_t::GPIO_NUM_6;
    static constexpr uint8_t I2C_PROBE_TIMEOUT_MS = 50;

    static inline ::i2c_master_bus_handle_t _masterBus = nullptr;

    ::i2c_master_dev_handle_t _deviceHandle;
    uint16_t _addressDevice;

public:
    I2C(uint16_t addressDevice)
    {
        _addressDevice = addressDevice;
    };

    void Init()
    {
        if (_masterBus == nullptr)
        {
            ::i2c_master_bus_config_t config{

                .i2c_port = I2C_PORT,
                .sda_io_num = I2C_SDA_PIN,
                .scl_io_num = I2C_SCL_PIN,
                .clk_source = soc_periph_i2c_clk_src_t::I2C_CLK_SRC_DEFAULT,
                .glitch_ignore_cnt = 7,
                .intr_priority = 0,
                .trans_queue_depth = 0,
                .flags = {.enable_internal_pullup = true, .allow_pd = false}};

            ::esp_err_t status = ::i2c_new_master_bus(&config, &_masterBus);
        }

        ::i2c_device_config_t confiDevice{
            .dev_addr_length = i2c_addr_bit_len_t::I2C_ADDR_BIT_LEN_7,
            .device_address = _addressDevice,
            .scl_speed_hz = 400000,
            .scl_wait_us = 0,
            .flags = {.disable_ack_check = false}};

        i2c_master_bus_add_device(_masterBus, &confiDevice, &_deviceHandle);
    };

    void I2cWrite(uint8_t data)
    {
        i2c_master_transmit(_deviceHandle, &data, sizeof(data), I2C_PROBE_TIMEOUT_MS);
    }

    void I2cWriteReg(uint8_t regAddr, uint8_t data)
    {
        uint8_t regAndData[2] = {regAddr, data};
        i2c_master_transmit(_deviceHandle, regAndData, sizeof(regAndData), I2C_PROBE_TIMEOUT_MS);
    }

    template <uint8_t N>
    std::array<uint8_t, N> I2cRead()
    {
        std::array<uint8_t, N> retData{};
        i2c_master_receive(_deviceHandle, retData.data(), N, I2C_PROBE_TIMEOUT_MS);
        return retData;
    }

    template <uint8_t N>
    std::array<uint8_t, N> I2cReadRegs(uint8_t startReg)
    {
        std::array<uint8_t, N> retData{};
        i2c_master_transmit_receive(_deviceHandle, &startReg, sizeof(startReg), retData.data(), N, I2C_PROBE_TIMEOUT_MS);

        return retData;
    }
};
