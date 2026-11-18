#include "../include/foo.h"
#include <iostream>

/**
 * @brief Constructor implementation
 */
Foo::Foo(const std::string& name, std::shared_ptr<Device> device)
    : SequenceBuilder(), m_name(name), m_state(0), m_device(device) {
    std::cout << "[Foo] Constructor: Created workflow '" << m_name << "'" << std::endl;
    if (m_device) {
        std::cout << "[Foo] Device attached: '" << m_device->getName() << "'" << std::endl;
    } else {
        std::cout << "[Foo] No device attached" << std::endl;
    }
}

/**
 * @brief Initializes the default FSM sequence with three steps
 */
void Foo::initializeSequence() {
    std::cout << "[Foo::initializeSequence] Setting up FSM sequence for '" << m_name << "'" << std::endl;

    // Step 1: Initialize device
    this->createStep(
        "initialize",
        [this]() { return this->step_initialize(); },
        TagType::STANDARD
    );

    // Step 2: Process with device
    this->createStep(
        "process",
        [this]() { return this->step_process(); },
        TagType::CUSTOM,
        "device_processing"
    );

    // Step 3: Finalize device
    this->createStep(
        "finalize",
        [this]() { return this->step_finalize(); },
        TagType::STANDARD
    );

    std::cout << "[Foo::initializeSequence] Sequence initialized with 3 default steps" << std::endl;
    std::cout << "[Foo::initializeSequence] Access inherited methods: createStep(), getStep(), deleteStep(), modifyStepFunctionality(), executeSequence(), etc." << std::endl;
}

/**
 * @brief Convenience method to run the workflow
 */
void Foo::runWorkflow() {
    std::cout << "[Foo::runWorkflow] Starting FSM execution for '" << m_name << "'" << std::endl;
    this->executeSequence("initialize");
    std::cout << "[Foo::runWorkflow] Workflow completed" << std::endl;
}

/**
 * @brief Step 1: Initialize the device and Foo workflow
 * @return Name of the next step ("process")
 */
std::string Foo::step_initialize() {
    std::cout << "  [Foo::step_initialize] Initializing '" << m_name << "'" << std::endl;
    m_state = 1;
    std::cout << "  [Foo::step_initialize] State set to: " << m_state << std::endl;
    
    if (m_device) {
        m_device->initialize();
    }
    
    return "process";
}

/**
 * @brief Step 2: Process with the device
 * @return Name of the next step ("finalize")
 */
std::string Foo::step_process() {
    std::cout << "  [Foo::step_process] Processing '" << m_name << "'" << std::endl;
    m_state += 10;
    std::cout << "  [Foo::step_process] State incremented to: " << m_state << std::endl;
    
    if (m_device) {
        m_device->operate();
    }
    
    return "finalize";
}

/**
 * @brief Step 3: Finalize the device and Foo workflow
 * @return Empty string to end the sequence
 */
std::string Foo::step_finalize() {
    std::cout << "  [Foo::step_finalize] Finalizing '" << m_name << "'" << std::endl;
    m_state += 100;
    std::cout << "  [Foo::step_finalize] Final state: " << m_state << std::endl;
    
    if (m_device) {
        std::cout << "  [Foo::step_finalize] Device status before finalize: " 
                  << m_device->getStatus() << std::endl;
        m_device->finalize();
        std::cout << "  [Foo::step_finalize] Device status after finalize: " 
                  << m_device->getStatus() << std::endl;
    }
    
    return "";
}
