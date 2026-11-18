#pragma once

#include <string>
#include <iostream>

/**
 * @class Device
 * @brief Abstract base class for device operations
 * 
 * Represents a generic device that can be initialized, operated, and finalized
 */
class Device {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Device() = default;

    /**
     * @brief Initializes the device
     */
    virtual void initialize() = 0;

    /**
     * @brief Performs an operation on the device
     */
    virtual void operate() = 0;

    /**
     * @brief Finalizes the device
     */
    virtual void finalize() = 0;

    /**
     * @brief Gets the device name
     */
    virtual const std::string& getName() const = 0;

    /**
     * @brief Gets the device status
     */
    virtual int getStatus() const = 0;
};

/**
 * @class SimpleDevice
 * @brief Simple device implementation
 */
class SimpleDevice : public Device {
private:
    std::string m_name;
    int m_status;

public:
    /**
     * @brief Constructor
     * @param name Device name
     */
    explicit SimpleDevice(const std::string& name = "SimpleDevice");

    /**
     * @brief Destructor
     */
    ~SimpleDevice() override;

    /**
     * @brief Initializes the device
     */
    void initialize() override;

    /**
     * @brief Performs an operation
     */
    void operate() override;

    /**
     * @brief Finalizes the device
     */
    void finalize() override;

    /**
     * @brief Gets the device name
     */
    const std::string& getName() const override { return m_name; }

    /**
     * @brief Gets the device status
     */
    int getStatus() const override { return m_status; }

    /**
     * @brief Sets the device status
     */
    void setStatus(int status) { m_status = status; }
};
