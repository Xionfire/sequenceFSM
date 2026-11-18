#pragma once

#include "SequenceFSMHelper.h"
#include "device.h"
#include <iostream>
#include <string>
#include <memory>

/**
 * @class Foo
 * @brief A workflow class that inherits from SequenceBuilder for FSM management with Device support
 * 
 * This class inherits from SequenceBuilder to provide full FSM capabilities:
 * - Direct access to addStep(), modifyStep(), executeSequence(), etc.
 * - Integration with Device for workflow operations
 * - State management for Foo workflow
 */
class Foo : public SequenceBuilder {
private:
    std::string m_name;
    int m_state;
    std::shared_ptr<Device> m_device;

public:
    /**
     * @brief Constructor
     * @param name Name of the Foo workflow
     * @param device Shared pointer to a Device instance (optional)
     */
    Foo(const std::string& name = "DefaultFoo", std::shared_ptr<Device> device = nullptr);

    /**
     * @brief Destructor
     */
    ~Foo() = default;

    /**
     * @brief Initializes the default FSM sequence
     * Creates three default steps: initialize, process, finalize
     * Can be extended with additional custom steps via addStep()
     */
    void initializeSequence();

    /**
     * @brief Convenience method to run the workflow starting from the first step
     */
    void runWorkflow();

    /**
     * @brief Gets the name of the Foo workflow
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Gets the current workflow state
     */
    int getState() const { return m_state; }

    /**
     * @brief Sets the workflow state
     */
    void setState(int value) { m_state = value; }

    /**
     * @brief Gets the device
     */
    std::shared_ptr<Device> getDevice() const { return m_device; }

private:
    /**
     * @brief Sets the device
     */
    void setDevice(std::shared_ptr<Device> device) { m_device = device; }

    // FSM Step callbacks (bound to the workflow)
    std::string step_initialize();
    std::string step_process();
    std::string step_finalize();
};
