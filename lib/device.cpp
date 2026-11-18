#include "../include/device.h"

/**
 * @brief SimpleDevice constructor
 */
SimpleDevice::SimpleDevice(const std::string& name)
    : m_name(name), m_status(0) {
    std::cout << "[SimpleDevice] Created device: '" << m_name << "'" << std::endl;
}

/**
 * @brief SimpleDevice destructor
 */
SimpleDevice::~SimpleDevice() {
    std::cout << "[SimpleDevice] Destroyed device: '" << m_name << "'" << std::endl;
}

/**
 * @brief Initializes the device
 */
void SimpleDevice::initialize() {
    m_status = 1;
    std::cout << "  [SimpleDevice::initialize] Device '" << m_name 
              << "' initialized. Status: " << m_status << std::endl;
}

/**
 * @brief Performs an operation on the device
 */
void SimpleDevice::operate() {
    if (m_status > 0) {
        m_status += 10;
        std::cout << "  [SimpleDevice::operate] Device '" << m_name 
                  << "' operating. Status: " << m_status << std::endl;
    } else {
        std::cout << "  [SimpleDevice::operate] Device '" << m_name 
                  << "' cannot operate - not initialized!" << std::endl;
    }
}

/**
 * @brief Finalizes the device
 */
void SimpleDevice::finalize() {
    m_status = -1;
    std::cout << "  [SimpleDevice::finalize] Device '" << m_name 
              << "' finalized. Status: " << m_status << std::endl;
}
