#include "../include/SequenceFSMHelper.h"
#include <iostream>
#include <memory>

/**
 * @file example_async_steps.cpp
 * @brief Demonstrates launching the same step multiple times "in parallel"
 *        and gating the sequence on the completion of specific step ids.
 *
 * IMPORTANT: this is cooperative / scan-cycle based, NOT real OS threading.
 * It is designed to run safely inside a PLC-style deterministic scan loop:
 * each executeSequence() iteration represents one "scan" and advances every
 * running async instance by exactly one tick via tickAsyncSteps().
 *
 * Shows:
 * 1. Defining ONE step blueprint and reusing it for several parallel
 *    instances (no copy/paste x times), each with its OWN independent state
 * 2. launchStepAsyncMany() to launch x instances, auto-tracked by id
 *    ("work_0", "work_1", ...)
 * 3. isAsyncStepFinished()/areAsyncStepsFinished() to poll a specific id
 * 4. createAsyncJoinStep() so the FSM only moves on once the chosen ids
 *    are finished ("if step id x is finished, go ahead")
 */

int main() {
    std::cout << "========== Async Multi-Instance Steps Example ==========\n" << std::endl;

    SequenceBuilder builder;

    // One single blueprint, instantiated 4 times below with independent state
    // (a per-instance "remaining ticks" counter) - no copy/paste x4.
    builder.defineAsyncStep("work", []() -> SequenceStep::StepCallback {
        static int nextIndex = 0;
        auto index = std::make_shared<int>(nextIndex++);
        auto remainingTicks = std::make_shared<int>(2 + (*index) * 2);  // each instance "takes" a different duration

        return [index, remainingTicks]() -> std::string {
            std::cout << "  [work_" << *index << "] tick, " << *remainingTicks << " remaining" << std::endl;
            if (--(*remainingTicks) > 0) {
                return "work";  // not done yet: keep running, ticked again next scan
            }
            std::cout << "  [work_" << *index << "] finished" << std::endl;
            return "";  // this instance is done
        };
    });

    builder.createStep("start", [&builder]() {
        std::cout << "[start] Launching 4 concurrent instances of 'work' (no copy/paste x4)" << std::endl;
        auto ids = builder.launchStepAsyncMany("work", 4, "work");
        std::cout << "[start] Tracking ids: ";
        for (const auto& id : ids) std::cout << id << " ";
        std::cout << std::endl;
        return "join";
    }, TagType::STANDARD);

    // Only continues to "done" once work_0..work_3 are all finished.
    // Non-blocking: on each scan where they aren't finished, it stays on
    // itself (re-checked next scan), so tickAsyncSteps() keeps advancing them.
    builder.createAsyncJoinStep("join",
        {"work_0", "work_1", "work_2", "work_3"},
        "done");

    builder.createStep("done", [&builder]() {
        std::cout << "[done] All 4 async 'work' instances are finished -> going ahead" << std::endl;
        std::cout << "[done] work_2 finished? " << (builder.isAsyncStepFinished("work_2") ? "yes" : "no") << std::endl;
        return "";
    }, TagType::STANDARD);

    // Each loop iteration inside executeSequence() represents one scan cycle:
    // it ticks all async instances once, then executes the current main step.
    builder.executeSequence("start", /*maxSteps=*/100);

    std::cout << "\n========== End ==========\n" << std::endl;
    return 0;
}
