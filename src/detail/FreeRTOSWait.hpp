#pragma once

#include <cstdint>
#include <limits>

#include <FreeRTOS.h>
#include <task.h>

#include <ESPressio_Platform.hpp>

namespace ESPressio::Platform::FreeRTOS::Detail {

    /// Number of nanoseconds in one second.
    inline constexpr std::uint64_t NanosecondsPerSecond = 1000000000ULL;

    /// FreeRTOS tick resolution rounded upward to whole nanoseconds.
    inline constexpr std::uint64_t WaitResolutionNanoseconds =
        (NanosecondsPerSecond + static_cast<std::uint64_t>(configTICK_RATE_HZ) - 1ULL) /
        static_cast<std::uint64_t>(configTICK_RATE_HZ);


    /// Converts an ESPressio wait request into FreeRTOS ticks without early timeout.
    inline TickType_t ToTicks(
        ESPressio::Platform::Synchronization::WaitTimeout timeout
    ) noexcept {
        if (timeout.IsForever()) { return portMAX_DELAY; }

        if (timeout.IsNoWait()) { return static_cast<TickType_t>(0U); }

        const auto nanoseconds = timeout.Nanoseconds();
        const auto wholeSeconds = nanoseconds / NanosecondsPerSecond;
        const auto remainingNanoseconds = nanoseconds % NanosecondsPerSecond;
        const auto tickRate = static_cast<std::uint64_t>(configTICK_RATE_HZ);
        const auto maximumFiniteTicks = static_cast<std::uint64_t>(portMAX_DELAY) - 1ULL;

        if (wholeSeconds > maximumFiniteTicks / tickRate) {
            return static_cast<TickType_t>(maximumFiniteTicks);
        }

        auto ticks = wholeSeconds * tickRate;
        const auto fractionalTicks = (
            remainingNanoseconds * tickRate + NanosecondsPerSecond - 1ULL
        ) / NanosecondsPerSecond;

        if (fractionalTicks > maximumFiniteTicks - ticks) {
            return static_cast<TickType_t>(maximumFiniteTicks);
        }

        ticks += fractionalTicks;

        if (ticks == 0ULL) { ticks = 1ULL; }

        if (ticks > maximumFiniteTicks) { ticks = maximumFiniteTicks; }

        return static_cast<TickType_t>(ticks);
    }


    /// Tracks one total FreeRTOS wait budget across multiple native acquisitions.
    class WaitBudget final {

        private:

            // Wait budget state.

            /// Original ESPressio wait request.
            ESPressio::Platform::Synchronization::WaitTimeout _timeout;

            /// Native finite tick budget.
            TickType_t _ticks;

            /// Tick coordinate captured when the budget was created.
            TickType_t _start;

        public:

            // Construction.

            /// Creates a wait budget from one ESPressio timeout.
            explicit WaitBudget(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept :
                _timeout(timeout),
                _ticks(ToTicks(timeout)),
                _start(xTaskGetTickCount()) {}


            // Remaining-time inspection.

            /// Returns the native wait duration remaining in this budget.
            TickType_t Remaining() const noexcept {
                if (_timeout.IsForever()) { return portMAX_DELAY; }

                if (_timeout.IsNoWait()) { return static_cast<TickType_t>(0U); }

                const auto elapsed = static_cast<TickType_t>(
                    xTaskGetTickCount() - _start
                );

                if (elapsed >= _ticks) { return static_cast<TickType_t>(0U); }

                return static_cast<TickType_t>(
                    _ticks - elapsed
                );
            }

    };

} // ESPressio::Platform::FreeRTOS::Detail
