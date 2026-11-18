#include "../include/foo.h"
#include "../include/device.h"
#include <iostream>
#include <memory>

/**
 * @brief Simple example: Create a Device, pass it to Foo via shared_ptr,
 *        and modify the device through Foo's workflow
 */
int main() {
    std::cout << "========== Foo Modifies a Device ==========\n" << std::endl;

    // Step 1: Create a SimpleDevice
    auto device = std::make_shared<SimpleDevice>("MyDevice");
    std::cout << "Device created: " << device->getName() << std::endl;
    std::cout << "Initial status: " << device->getStatus() << "\n" << std::endl;

    // Step 2: Pass device to Foo via constructor
    Foo workflow("MyWorkflow", device);
    std::cout << "Workflow created with device\n" << std::endl;

    // Step 3: Initialize the workflow sequence
    workflow.initializeSequence();

    // Step 4: Run the workflow (which modifies the device)
    std::cout << "--- Workflow Execution ---" << std::endl;
    workflow.runWorkflow();

    // Step 5: Check the device status after workflow execution
    std::cout << "\n--- Results After Workflow ---" << std::endl;
    std::cout << "Device name: " << device->getName() << std::endl;
    std::cout << "Device final status: " << device->getStatus() << std::endl;

    std::cout << "\n========== End ==========\n" << std::endl;
    return 0;
}
