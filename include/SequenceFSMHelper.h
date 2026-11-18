#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <stdexcept>

/**
 * @enum TagType
 * @brief Enumeration of available tag types
 */
enum class TagType {
    STANDARD,  // Predefined standard tag
    CUSTOM     // Customized tag
};

/**
 * @class SequenceStep
 * @brief Represents a step in an FSM sequence
 * 
 * A sequence step has:
 * - A unique name
 * - A functionality (callback)
 * - A tag (standard or custom)
 * - Transition happens via the return value of the function
 * 
 * Each function must return the name of the next step.
 * An empty string "" means the end of the sequence.
 */
class SequenceStep {
public:
    // Callback that returns the name of the next step
    using StepCallback = std::function<std::string()>;

    /**
     * @brief Constructor
     * @param name Unique name of the step
     * @param functionality Callback function to execute (must return the name of the next step)
     * @param tag Tag type (STANDARD or CUSTOM)
     * @param tagValue Tag value (optional, used for CUSTOM tags)
     */
    SequenceStep(const std::string& name,
                 StepCallback functionality,
                 TagType tag,
                 const std::string& tagValue = "")
        : m_name(name),
          m_functionality(functionality),
          m_tag(tag),
          m_tagValue(tagValue) {
        if (name.empty()) {
            throw std::invalid_argument("Step name cannot be empty");
        }
    }

    /**
     * @brief Destructor
     */
    ~SequenceStep() = default;

    // Getters
    const std::string& getName() const { return m_name; }
    
    TagType getTagType() const { return m_tag; }
    
    const std::string& getTagValue() const { return m_tagValue; }
    
    /**
     * @brief Executes the step functionality and returns the name of the next step
     * @return Name of the next step, or "" if end of sequence
     */
    std::string execute() const {
        if (m_functionality) {
            return m_functionality();
        }
        return "";  // End of sequence by default
    }

    // Setters
    /**
     * @brief Modifies the step functionality
     * @param newFunctionality New callback function
     */
    void setFunctionality(StepCallback newFunctionality) {
        m_functionality = newFunctionality;
    }

    /**
     * @brief Modifies the step tag
     * @param tag New tag type
     * @param tagValue New tag value (for CUSTOM tags)
     */
    void setTag(TagType tag, const std::string& tagValue = "") {
        m_tag = tag;
        m_tagValue = tagValue;
    }

    /**
     * @brief Returns a complete description of the step
     */
    std::string describe() const {
        std::string description = "Step: " + m_name;
        description += " | Tag: ";
        description += (m_tag == TagType::STANDARD ? "STANDARD" : "CUSTOM");
        if (!m_tagValue.empty()) {
            description += "(" + m_tagValue + ")";
        }
        return description;
    }

private:
    std::string m_name;                    ///< Unique name of the step
    StepCallback m_functionality;          ///< Callback function of the step (returns the name of the next step)
    TagType m_tag;                         ///< Tag type
    std::string m_tagValue;                ///< Tag value (for CUSTOM)
};

/**
 * @class SequenceBuilder
 * @brief FSM sequence manager allowing to create, modify and delete steps
 */
class SequenceBuilder {
public:
    using StepPtr = std::shared_ptr<SequenceStep>;

    /**
     * @brief Default constructor
     */
    SequenceBuilder() = default;

    /**
     * @brief Destructor
     */
    ~SequenceBuilder() = default;

    /**
     * @brief Creates a new sequence step
     * @param name Unique name of the step
     * @param functionality Callback function to execute
     * @param tag Tag type (STANDARD or CUSTOM)
     * @param tagValue Tag value (optional)
     * @return Shared pointer to the created step
     * @throw std::invalid_argument if a step with this name already exists
     */
    StepPtr createStep(const std::string& name,
                       SequenceStep::StepCallback functionality,
                       TagType tag,
                       const std::string& tagValue = "") {
        if (m_steps.find(name) != m_steps.end()) {
            throw std::invalid_argument("A step with the name '" + name + "' already exists");
        }

        auto step = std::make_shared<SequenceStep>(name, functionality, tag, tagValue);
        m_steps[name] = step;
        return step;
    }

    /**
     * @brief Retrieves an existing step by its name
     * @param name Name of the step
     * @return Shared pointer to the step
     * @throw std::out_of_range if the step does not exist
     */
    StepPtr getStep(const std::string& name) const {
        auto it = m_steps.find(name);
        if (it == m_steps.end()) {
            throw std::out_of_range("The step '" + name + "' does not exist");
        }
        return it->second;
    }

    /**
     * @brief Deletes a step from the sequence
     * @param name Name of the step to delete
     * @throw std::out_of_range if the step does not exist
     */
    void deleteStep(const std::string& name) {
        auto it = m_steps.find(name);
        if (it == m_steps.end()) {
            throw std::out_of_range("The step '" + name + "' does not exist");
        }
        m_steps.erase(it);
    }

    /**
     * @brief Modifies the functionality of an existing step
     * 
     * If the step was of type STANDARD, it will automatically be converted to CUSTOM.
     * 
     * @param stepName Name of the step
     * @param newFunctionality New callback function
     * @param customTagValue Value of the CUSTOM tag (used if the step was STANDARD)
     * @throw std::out_of_range if the step does not exist
     */
    void modifyStepFunctionality(const std::string& stepName,
                                 SequenceStep::StepCallback newFunctionality,
                                 const std::string& customTagValue = "modified") {
        auto step = getStep(stepName);
        step->setFunctionality(newFunctionality);
        
        // If the step was STANDARD, force it to become CUSTOM
        if (step->getTagType() == TagType::STANDARD) {
            step->setTag(TagType::CUSTOM, customTagValue);
        }
    }

    /**
     * @brief Modifies the tag of a step
     * @param stepName Name of the step
     * @param tag New tag type
     * @param tagValue New tag value
     * @throw std::out_of_range if the step does not exist
     */
    void modifyStepTag(const std::string& stepName,
                       TagType tag,
                       const std::string& tagValue = "") {
        auto step = getStep(stepName);
        step->setTag(tag, tagValue);
    }

    /**
     * @brief Executes a sequence starting from a given step
     * 
     * The transition happens automatically via the return value of each step.
     * Each step must return the name of the next step via its callback.
     * An empty string means the end of the sequence.
     * 
     * @param startStepName Name of the starting step
     * @param maxSteps Maximum number of steps to execute (limit to avoid infinite loops)
     * @throw std::out_of_range if the starting step does not exist
     */
    void executeSequence(const std::string& startStepName, size_t maxSteps = 1000) const {
        auto currentStepPtr = getStep(startStepName);
        size_t stepCount = 0;

        while (currentStepPtr && stepCount < maxSteps) {
            // Execute the step and get the name of the next step
            std::string nextStepName = currentStepPtr->execute();
            
            if (nextStepName.empty()) {
                // End of sequence
                break;
            }

            try {
                currentStepPtr = getStep(nextStepName);
            } catch (const std::out_of_range&) {
                throw std::out_of_range("The step '" + nextStepName + "' does not exist (returned by '" + 
                                       currentStepPtr->getName() + "')");
            }

            ++stepCount;
        }

        if (stepCount >= maxSteps) {
            throw std::runtime_error("Sequence stopped: maximum number of steps (" + 
                                    std::to_string(maxSteps) + ") reached (infinite loop?)");
        }
    }

    /**
     * @brief Returns the number of steps in the sequence
     */
    size_t getStepCount() const { return m_steps.size(); }

    /**
     * @brief Checks if a step exists
     * @param name Name of the step
     */
    bool hasStep(const std::string& name) const {
        return m_steps.find(name) != m_steps.end();
    }

    /**
     * @brief Returns a reference to the internal steps map (for exporters/utilities)
     * @return Const reference to the steps map
     */
    const std::unordered_map<std::string, StepPtr>& getStepsMap() const {
        return m_steps;
    }

private:
    std::unordered_map<std::string, StepPtr> m_steps;  ///< Storage of steps by name
};
