#pragma once

#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
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
 * @enum AsyncStepState
 * @brief State of an asynchronously launched step instance
 */
enum class AsyncStepState {
    NOT_FOUND,  // No instance was launched with this id
    RUNNING,    // The instance is still executing
    FINISHED    // The instance has completed execution
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

    /// Factory producing one independent StepCallback per async step instance
    using StepFactory = std::function<SequenceStep::StepCallback()>;

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
    void executeSequence(const std::string& startStepName, size_t maxSteps = 1000) {
        auto currentStepPtr = getStep(startStepName);
        size_t stepCount = 0;

        while (currentStepPtr && stepCount < maxSteps) {
            // One iteration = one scan cycle: advance all running async
            // instances by one tick, then execute the current main step.
            tickAsyncSteps();

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

    // ========================================================================
    // Async multi-instance steps (no OS threads — PLC/scan-cycle safe)
    //
    // These methods let you define a step's logic ONCE as a "blueprint" and
    // then launch it several times concurrently, each launch ("instance")
    // tracked by a unique id and carrying its own independent state.
    //
    // There is NO real multithreading here: every instance is advanced
    // cooperatively by exactly one call per tickAsyncSteps() invocation,
    // just like a PLC function block is scanned once per cycle. This keeps
    // execution fully deterministic and safe for a real-time/PLC context.
    // executeSequence() already calls tickAsyncSteps() once per iteration;
    // if you drive the FSM yourself (your own scan loop), call
    // tickAsyncSteps() once per cycle in addition to executing your steps.
    // ========================================================================

    /**
     * @brief Defines a reusable step "blueprint" that can be instantiated
     *        multiple times via launchStepAsync()/launchStepAsyncMany().
     *
     * The factory is called once PER instance and must return a brand-new
     * StepCallback with its own captured state (counters, progress, etc.),
     * so that concurrent instances don't share/clobber each other's state.
     * Like a normal step, that callback should return "" when the instance
     * is finished, or its own name (or anything non-empty) to keep running
     * on the next tick.
     *
     * @param stepName Unique name identifying this blueprint
     * @param factory Function returning a fresh StepCallback for one instance
     * @throw std::invalid_argument if a blueprint with this name already exists
     */
    void defineAsyncStep(const std::string& stepName, StepFactory factory) {
        if (m_asyncFactories.find(stepName) != m_asyncFactories.end()) {
            throw std::invalid_argument("An async step blueprint named '" + stepName + "' already exists");
        }
        m_asyncFactories[stepName] = std::move(factory);
    }

    /**
     * @brief Instantiates a step blueprint and starts tracking it under an id
     *
     * @param stepName Name of a blueprint registered via defineAsyncStep()
     * @param instanceId Unique id used to track/query this instance later. If
     *        empty, an id is auto-generated as "<stepName>#<n>".
     * @return The instance id that was used, for isAsyncStepFinished() etc.
     * @throw std::out_of_range if the blueprint does not exist
     * @throw std::invalid_argument if the instance id is already in use
     */
    std::string launchStepAsync(const std::string& stepName, const std::string& instanceId = "") {
        auto factoryIt = m_asyncFactories.find(stepName);
        if (factoryIt == m_asyncFactories.end()) {
            throw std::out_of_range("No async step blueprint named '" + stepName + "' (call defineAsyncStep first)");
        }

        std::string id = instanceId.empty() ? generateInstanceId(stepName) : instanceId;
        if (m_asyncInstances.find(id) != m_asyncInstances.end()) {
            throw std::invalid_argument("An async step instance with id '" + id + "' already exists");
        }

        AsyncInstance instance;
        instance.blueprintName = stepName;
        instance.callback = factoryIt->second();  // independent state per instance
        instance.finished = false;
        m_asyncInstances.emplace(id, std::move(instance));
        return id;
    }

    /**
     * @brief Launches the SAME step blueprint multiple times concurrently,
     *        without writing it x times
     *
     * Example: builder.launchStepAsyncMany("process", 5) creates 5 independent
     * running instances of "process", tracked as "process_0" .. "process_4".
     *
     * @param stepName Name of a blueprint registered via defineAsyncStep()
     * @param count Number of concurrent instances to launch
     * @param idPrefix Prefix used to build each instance id (default: stepName)
     * @return Vector of instance ids, one per launched instance, in order
     */
    std::vector<std::string> launchStepAsyncMany(const std::string& stepName, size_t count,
                                                  const std::string& idPrefix = "") {
        std::vector<std::string> ids;
        ids.reserve(count);
        const std::string prefix = idPrefix.empty() ? stepName : idPrefix;
        for (size_t i = 0; i < count; ++i) {
            ids.push_back(launchStepAsync(stepName, prefix + "_" + std::to_string(i)));
        }
        return ids;
    }

    /**
     * @brief Advances every still-running async instance by exactly one call
     *        ("one scan"). No threads, no blocking: each instance's callback
     *        is called once; "" means it just finished, anything else means
     *        it keeps running and will be called again on the next tick.
     *
     * Called automatically once per iteration by executeSequence(). Call it
     * yourself once per cycle if you drive the FSM from your own scan loop.
     */
    void tickAsyncSteps() {
        for (auto& entry : m_asyncInstances) {
            AsyncInstance& instance = entry.second;
            if (instance.finished) {
                continue;
            }
            std::string result = instance.callback ? instance.callback() : "";
            if (result.empty()) {
                instance.finished = true;
            }
        }
    }

    /**
     * @brief Returns the current state (RUNNING/FINISHED/NOT_FOUND) of an async instance
     * @param instanceId Id returned by launchStepAsync()/launchStepAsyncMany()
     */
    AsyncStepState getAsyncStepState(const std::string& instanceId) const {
        auto it = m_asyncInstances.find(instanceId);
        if (it == m_asyncInstances.end()) {
            return AsyncStepState::NOT_FOUND;
        }
        return it->second.finished ? AsyncStepState::FINISHED : AsyncStepState::RUNNING;
    }

    /**
     * @brief Convenience check: is step id x finished?
     * @param instanceId Id returned by launchStepAsync()/launchStepAsyncMany()
     * @return true only if the instance exists and has finished
     */
    bool isAsyncStepFinished(const std::string& instanceId) const {
        return getAsyncStepState(instanceId) == AsyncStepState::FINISHED;
    }

    /**
     * @brief Checks whether ALL given async instance ids have finished
     * @param instanceIds Ids to check
     */
    bool areAsyncStepsFinished(const std::vector<std::string>& instanceIds) const {
        for (const auto& id : instanceIds) {
            if (!isAsyncStepFinished(id)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Checks whether an async instance id is currently tracked (running or finished)
     */
    bool hasAsyncStep(const std::string& instanceId) const {
        return m_asyncInstances.find(instanceId) != m_asyncInstances.end();
    }

    /**
     * @brief Removes an async instance from tracking (frees its captured state)
     * @param instanceId Id to clear; silently ignored if unknown
     */
    void clearAsyncStep(const std::string& instanceId) {
        m_asyncInstances.erase(instanceId);
    }

    /**
     * @brief Creates a synchronization step: it only transitions to
     *        nextStepName once every id in instanceIds has finished
     *        ("if step id x is finished, go ahead").
     *
     * This step is non-blocking by design (PLC-safe): on each scan where the
     * instances aren't all finished yet, it simply returns its own name so it
     * gets re-evaluated on the next scan, same as e.g. temperature_heat/cool
     * self-looping until a condition is met.
     *
     * @param name Unique name of the new synchronization step
     * @param instanceIds Instance ids (from launchStepAsync/launchStepAsyncMany) to wait for
     * @param nextStepName Step to transition to once all instanceIds have finished
     * @return Shared pointer to the created synchronization step
     */
    StepPtr createAsyncJoinStep(const std::string& name,
                                 std::vector<std::string> instanceIds,
                                 const std::string& nextStepName) {
        return createStep(name, [this, instanceIds, nextStepName, name]() -> std::string {
            if (areAsyncStepsFinished(instanceIds)) {
                return nextStepName;
            }
            return name;  // not finished yet: stay here, re-checked next scan
        }, TagType::CUSTOM, "async_join");
    }

private:
    /**
     * @struct AsyncInstance
     * @brief Tracks one running (or finished) instance of an async step blueprint
     */
    struct AsyncInstance {
        std::string blueprintName;            ///< Name of the blueprint this instance came from
        SequenceStep::StepCallback callback;  ///< This instance's own callback/state
        bool finished = false;                ///< Set once the callback returns ""
    };

    /**
     * @brief Generates a unique instance id for a step launched without an explicit id
     */
    std::string generateInstanceId(const std::string& stepName) {
        return stepName + "#" + std::to_string(m_asyncCounter++);
    }

    std::unordered_map<std::string, StepPtr> m_steps;  ///< Storage of steps by name

    std::unordered_map<std::string, StepFactory> m_asyncFactories;      ///< Blueprints by name
    std::unordered_map<std::string, AsyncInstance> m_asyncInstances;    ///< Running/finished instances by id
    size_t m_asyncCounter = 0;                                          ///< Counter for auto-generated ids
};
