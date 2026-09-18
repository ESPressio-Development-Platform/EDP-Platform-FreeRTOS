#pragma once

#include <atomic>
#include <cstdint>
#include <limits>

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#include <ESPressio_Platform.hpp>

#include "../detail/FreeRTOSWait.hpp"

namespace ESPressio::Platform::FreeRTOS::Execution {

    namespace Framework = ESPressio::System::CompositionFramework;

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( INCLUDE_vTaskDelete == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) && ( INCLUDE_xTaskGetCurrentTaskHandle == 1 )

    /// Vanilla FreeRTOS static execution-context provider.
    ///
    /// The created native task begins behind a private start gate. After the user entry function
    /// returns it publishes completion and suspends itself until the owner destroys the context.
    class ExecutionContextProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
        Framework::Provides<
            Framework::Offer<
                ESPressio::Platform::Execution::ExecutionContext,
                Framework::PropertyValue<ESPressio::Platform::Execution::CallerSuppliedStorage, true>,
                Framework::PropertyValue<ESPressio::Platform::Execution::SupportsPriority, true>,
                Framework::PropertyValue<ESPressio::Platform::Execution::SupportsProcessorAffinity, false>,
                Framework::PropertyValue<ESPressio::Platform::Execution::SupportsStackTelemetry, false>,
                Framework::PropertyValue<ESPressio::Platform::Execution::ControlStorageBytes, sizeof(StaticTask_t)>,
                Framework::PropertyValue<ESPressio::Platform::Execution::ControlStorageAlignment, alignof(StaticTask_t)>,
                Framework::PropertyValue<ESPressio::Platform::Execution::StackStorageAlignment, alignof(StackType_t)>,
                Framework::PropertyValue<ESPressio::Platform::Execution::StackAllocationGranularityBytes, sizeof(StackType_t)>,
                Framework::PropertyValue<
                    ESPressio::Platform::Execution::JoinWaitResolutionNanoseconds,
                    ESPressio::Platform::FreeRTOS::Detail::WaitResolutionNanoseconds
                >
            >
        >
    > {

        private:

            // Execution lifecycle state.

            /// Native FreeRTOS task handle while initialized.
            TaskHandle_t _handle = nullptr;

            /// User entry function invoked after Start.
            ESPressio::Platform::Execution::ExecutionEntry _entry = nullptr;

            /// User parameter passed to the entry function.
            void* _parameter = nullptr;

            /// Indicates whether the execution context was initialized.
            std::atomic<bool> _initialized{false};

            /// Indicates whether Start has been accepted.
            std::atomic<bool> _started{false};

            /// Indicates whether the user entry function has returned.
            std::atomic<bool> _completed{false};


            // Private lifecycle signals.

            /// Static storage for the start gate.
            StaticSemaphore_t _startStorage{};

            /// Static storage for the completion signal.
            StaticSemaphore_t _completionStorage{};

            /// Start-gate semaphore.
            SemaphoreHandle_t _startSignal;

            /// Completion semaphore.
            SemaphoreHandle_t _completionSignal;


            // Native execution entry.

            /// Bridges the FreeRTOS task entry into the ESPressio execution function.
            static void NativeEntry(
                void* parameter
            ) noexcept {
                auto* self = static_cast<ExecutionContextProvider*>(parameter);

                if (
                    self == nullptr ||
                    self->_startSignal == nullptr ||
                    self->_completionSignal == nullptr
                ) {
                    vTaskSuspend(
                        nullptr
                    );
                    return;
                }

                if (
                    xSemaphoreTake(
                        self->_startSignal,
                        portMAX_DELAY
                    ) != pdTRUE
                ) {
                    vTaskSuspend(
                        nullptr
                    );
                    return;
                }

                const auto entry = self->_entry;

                if (entry != nullptr) {
                    entry(
                        self->_parameter
                    );
                }

                self->_completed.store(
                    true,
                    std::memory_order_release
                );

                (void)xSemaphoreGive(
                    self->_completionSignal
                );

                for (;;) {
                    vTaskSuspend(
                        nullptr
                    );
                }
            }


            // Storage validation.

            /// Reports whether an address satisfies a required alignment.
            static bool IsAligned(
                const void* address,
                std::size_t alignment
            ) noexcept {
                return address != nullptr &&
                    reinterpret_cast<std::uintptr_t>(address) % alignment == 0U;
            }

        public:

            // Construction and lifetime.

            /// Creates an uninitialized static FreeRTOS execution provider.
            ExecutionContextProvider() noexcept :
                _startSignal(
                    xSemaphoreCreateBinaryStatic(
                        &_startStorage
                    )
                ),
                _completionSignal(
                    xSemaphoreCreateBinaryStatic(
                        &_completionStorage
                    )
                ) {}

            /// Prevents copying native execution state.
            ExecutionContextProvider(const ExecutionContextProvider&) = delete;

            /// Prevents copy assignment of native execution state.
            ExecutionContextProvider& operator =(const ExecutionContextProvider&) = delete;

            /// Prevents moving native execution state.
            ExecutionContextProvider(ExecutionContextProvider&&) = delete;

            /// Prevents move assignment of native execution state.
            ExecutionContextProvider& operator =(ExecutionContextProvider&&) = delete;


            // Initialization.

            /// Binds caller-owned stack/control storage and creates one start-gated native task.
            ESPressio::Platform::Execution::ExecutionInitializationResult Initialize(
                const ESPressio::Platform::Execution::ExecutionStorage& storage,
                const ESPressio::Platform::Execution::ExecutionConfiguration& configuration,
                ESPressio::Platform::Execution::ExecutionEntry entry,
                void* parameter
            ) noexcept {
                if (_initialized.load(std::memory_order_acquire)) {
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::AlreadyInitialized;
                }

                if (
                    _startSignal == nullptr ||
                    _completionSignal == nullptr
                ) {
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::ProviderFailure;
                }

                if (!configuration.Affinity.IsAny()) {
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::UnsupportedAffinity;
                }

                if (
                    configuration.Priority >= static_cast<std::uint32_t>(configMAX_PRIORITIES) ||
                    entry == nullptr
                ) {
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::InvalidConfiguration;
                }

                if (
                    storage.ControlBytes < sizeof(StaticTask_t) ||
                    storage.StackBytes < sizeof(StackType_t) ||
                    storage.StackBytes % sizeof(StackType_t) != 0U ||
                    !IsAligned(
                        storage.ControlAddress,
                        alignof(StaticTask_t)
                    ) ||
                    !IsAligned(
                        storage.StackAddress,
                        alignof(StackType_t)
                    )
                ) {
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::InvalidStorage;
                }

                const auto stackDepthBytes = storage.StackBytes / sizeof(StackType_t);

                if (
                    stackDepthBytes >
                    static_cast<std::size_t>(
                        std::numeric_limits<configSTACK_DEPTH_TYPE>::max()
                    )
                ) {
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::InvalidStorage;
                }

                while (
                    xSemaphoreTake(
                        _startSignal,
                        0U
                    ) == pdTRUE
                ) {}

                while (
                    xSemaphoreTake(
                        _completionSignal,
                        0U
                    ) == pdTRUE
                ) {}

                _entry = entry;
                _parameter = parameter;
                _started.store(
                    false,
                    std::memory_order_release
                );
                _completed.store(
                    false,
                    std::memory_order_release
                );

                const auto name = configuration.Name != nullptr
                    ? configuration.Name
                    : "EDP";

                _handle = xTaskCreateStatic(
                    &NativeEntry,
                    name,
                    static_cast<configSTACK_DEPTH_TYPE>(stackDepthBytes),
                    this,
                    static_cast<UBaseType_t>(configuration.Priority),
                    static_cast<StackType_t*>(storage.StackAddress),
                    static_cast<StaticTask_t*>(storage.ControlAddress)
                );

                if (_handle == nullptr) {
                    _entry = nullptr;
                    _parameter = nullptr;
                    return ESPressio::Platform::Execution::ExecutionInitializationResult::ProviderFailure;
                }

                _initialized.store(
                    true,
                    std::memory_order_release
                );

                return ESPressio::Platform::Execution::ExecutionInitializationResult::Succeeded;
            }


            // Lifecycle.

            /// Releases the private start gate and permits the native task to enter user code.
            ESPressio::Platform::Execution::ExecutionStartResult Start() noexcept {
                if (
                    !_initialized.load(std::memory_order_acquire) ||
                    _started.exchange(
                        true,
                        std::memory_order_acq_rel
                    )
                ) {
                    return ESPressio::Platform::Execution::ExecutionStartResult::InvalidState;
                }

                if (
                    xSemaphoreGive(
                        _startSignal
                    ) != pdTRUE
                ) {
                    _started.store(
                        false,
                        std::memory_order_release
                    );
                    return ESPressio::Platform::Execution::ExecutionStartResult::ProviderFailure;
                }

                return ESPressio::Platform::Execution::ExecutionStartResult::Succeeded;
            }

            /// Waits until the user execution entry has returned.
            ESPressio::Platform::Execution::ExecutionJoinResult Join(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (
                    !_initialized.load(std::memory_order_acquire) ||
                    !_started.load(std::memory_order_acquire)
                ) {
                    return ESPressio::Platform::Execution::ExecutionJoinResult::InvalidState;
                }

                if (IsCurrentContext()) {
                    return ESPressio::Platform::Execution::ExecutionJoinResult::SelfJoin;
                }

                if (_completed.load(std::memory_order_acquire)) {
                    return ESPressio::Platform::Execution::ExecutionJoinResult::Succeeded;
                }

                return xSemaphoreTake(
                    _completionSignal,
                    ESPressio::Platform::FreeRTOS::Detail::ToTicks(
                        timeout
                    )
                ) == pdTRUE
                    ? ESPressio::Platform::Execution::ExecutionJoinResult::Succeeded
                    : ESPressio::Platform::Execution::ExecutionJoinResult::TimedOut;
            }

            /// Destroys an unstarted context or a context whose user entry has already completed.
            ESPressio::Platform::Execution::ExecutionDestroyResult Destroy() noexcept {
                if (!_initialized.load(std::memory_order_acquire) || _handle == nullptr) {
                    return ESPressio::Platform::Execution::ExecutionDestroyResult::InvalidState;
                }

                if (
                    _started.load(std::memory_order_acquire) &&
                    !_completed.load(std::memory_order_acquire)
                ) {
                    return ESPressio::Platform::Execution::ExecutionDestroyResult::InvalidState;
                }

                vTaskDelete(
                    _handle
                );

                _handle = nullptr;
                _entry = nullptr;
                _parameter = nullptr;
                _completed.store(
                    false,
                    std::memory_order_release
                );
                _started.store(
                    false,
                    std::memory_order_release
                );
                _initialized.store(
                    false,
                    std::memory_order_release
                );

                return ESPressio::Platform::Execution::ExecutionDestroyResult::Succeeded;
            }


            // Execution inspection.

            /// Reports whether the caller is currently executing inside this native context.
            bool IsCurrentContext() const noexcept {
                return _handle != nullptr &&
                    xTaskGetCurrentTaskHandle() == _handle;
            }

            /// Reports that vanilla FreeRTOS stack telemetry is not normalized to bytes by this provider.
            ESPressio::Platform::Execution::ExecutionStackTelemetry GetStackTelemetry() const noexcept {
                return {};
            }

            /// Yields the current native FreeRTOS execution context.
            static void Yield() noexcept {
                taskYIELD();
            }

    };


    /// Compile-time validation of the vanilla FreeRTOS ExecutionContext provider.
    using ExecutionContextContract = ESPressio::Platform::Execution::Detail::ExecutionContextProviderTraits<ExecutionContextProvider>;

#endif

} // ESPressio::Platform::FreeRTOS::Execution
