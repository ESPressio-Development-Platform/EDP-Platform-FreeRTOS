#pragma once

#include <cstdint>

#include "../detail/FreeRTOSHeaders.hpp"

#include <ESPressio_Platform.hpp>

#include "../detail/FreeRTOSWait.hpp"

namespace ESPressio::Platform::FreeRTOS::Synchronization {

    namespace Framework = ESPressio::System::CompositionFramework;

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )

    /// FreeRTOS static-mutex provider.
    class MutexProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                ESPressio::Platform::Synchronization::Mutex,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::MutexWaitResolutionNanoseconds,
                    ESPressio::Platform::FreeRTOS::Detail::WaitResolutionNanoseconds
                >
            >
        >
    > {

        private:

            // Native synchronization state.

            /// Caller-owned FreeRTOS mutex storage embedded in this provider object.
            StaticSemaphore_t _storage{};

            /// Native FreeRTOS mutex handle.
            SemaphoreHandle_t _handle;

        public:

            // Construction and lifetime.

            /// Creates an unlocked static FreeRTOS mutex.
            MutexProvider() noexcept :
                _handle(
                    xSemaphoreCreateMutexStatic(
                        &_storage
                    )
                ) {}

            /// Prevents copying mutex state.
            MutexProvider(const MutexProvider&) = delete;

            /// Prevents copy assignment of mutex state.
            MutexProvider& operator =(const MutexProvider&) = delete;

            /// Prevents moving mutex state.
            MutexProvider(MutexProvider&&) = delete;

            /// Prevents move assignment of mutex state.
            MutexProvider& operator =(MutexProvider&&) = delete;


            // Lock operations.

            /// Acquires the mutex according to the requested wait policy.
            ESPressio::Platform::Synchronization::LockAcquireResult Acquire(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::ProviderFailure;
                }

                ESPressio::Platform::FreeRTOS::Detail::WaitBudget budget(timeout);

                return budget.Take(
                    _handle
                )
                    ? ESPressio::Platform::Synchronization::LockAcquireResult::Acquired
                    : ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
            }

            /// Releases the mutex owned by the current execution context.
            ESPressio::Platform::Synchronization::LockReleaseResult Release() noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::LockReleaseResult::ProviderFailure;
                }

                return xSemaphoreGive(
                    _handle
                ) == pdTRUE
                    ? ESPressio::Platform::Synchronization::LockReleaseResult::Released
                    : ESPressio::Platform::Synchronization::LockReleaseResult::InvalidOwnership;
            }

    };


    /// Compile-time validation of the FreeRTOS Mutex provider.
    using MutexContract = ESPressio::Platform::Synchronization::Detail::MutexProviderTraits<MutexProvider>;

#endif


#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_RECURSIVE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )

    /// FreeRTOS static recursive-mutex provider.
    class RecursiveMutexProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                ESPressio::Platform::Synchronization::RecursiveMutex,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::RecursiveMutexWaitResolutionNanoseconds,
                    ESPressio::Platform::FreeRTOS::Detail::WaitResolutionNanoseconds
                >
            >
        >
    > {

        private:

            // Native synchronization state.

            /// Caller-owned FreeRTOS recursive-mutex storage embedded in this provider object.
            StaticSemaphore_t _storage{};

            /// Native FreeRTOS recursive-mutex handle.
            SemaphoreHandle_t _handle;

        public:

            // Construction and lifetime.

            /// Creates an unlocked static FreeRTOS recursive mutex.
            RecursiveMutexProvider() noexcept :
                _handle(
                    xSemaphoreCreateRecursiveMutexStatic(
                        &_storage
                    )
                ) {}

            /// Prevents copying mutex state.
            RecursiveMutexProvider(const RecursiveMutexProvider&) = delete;

            /// Prevents copy assignment of mutex state.
            RecursiveMutexProvider& operator =(const RecursiveMutexProvider&) = delete;

            /// Prevents moving mutex state.
            RecursiveMutexProvider(RecursiveMutexProvider&&) = delete;

            /// Prevents move assignment of mutex state.
            RecursiveMutexProvider& operator =(RecursiveMutexProvider&&) = delete;


            // Lock operations.

            /// Acquires the recursive mutex according to the requested wait policy.
            ESPressio::Platform::Synchronization::LockAcquireResult Acquire(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::ProviderFailure;
                }

                ESPressio::Platform::FreeRTOS::Detail::WaitBudget budget(timeout);

                return budget.TakeRecursive(
                    _handle
                )
                    ? ESPressio::Platform::Synchronization::LockAcquireResult::Acquired
                    : ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
            }

            /// Releases one recursive acquisition owned by the current execution context.
            ESPressio::Platform::Synchronization::LockReleaseResult Release() noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::LockReleaseResult::ProviderFailure;
                }

                return xSemaphoreGiveRecursive(
                    _handle
                ) == pdTRUE
                    ? ESPressio::Platform::Synchronization::LockReleaseResult::Released
                    : ESPressio::Platform::Synchronization::LockReleaseResult::InvalidOwnership;
            }

    };


    /// Compile-time validation of the FreeRTOS RecursiveMutex provider.
    using RecursiveMutexContract = ESPressio::Platform::Synchronization::Detail::RecursiveMutexProviderTraits<RecursiveMutexProvider>;

#endif


#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )

    /// FreeRTOS reader/writer lock assembled from static mutex primitives.
    ///
    /// New readers pass through a writer turnstile so a waiting writer can prevent reader starvation.
    class ReadWriteMutexProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                ESPressio::Platform::Synchronization::ReadWriteMutex,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::ReadWriteMutexWaitResolutionNanoseconds,
                    ESPressio::Platform::FreeRTOS::Detail::WaitResolutionNanoseconds
                >
            >
        >
    > {

        private:

            // Native synchronization storage.

            /// Protects the reader-count transition.
            StaticSemaphore_t _readerGuardStorage{};

            /// Serializes writers and gates new readers behind waiting writers.
            StaticSemaphore_t _turnstileStorage{};

            /// Stores the binary semaphore excluding writers while readers are active.
            StaticSemaphore_t _resourceStorage{};

            /// Reader-count mutex.
            SemaphoreHandle_t _readerGuard;

            /// Writer turnstile mutex.
            SemaphoreHandle_t _turnstile;

            /// Shared resource binary semaphore; ownership may pass between distinct reader tasks.
            SemaphoreHandle_t _resource;


            // Logical synchronization state.

            /// Number of active shared readers.
            std::uint32_t _readers = 0U;

            /// Indicates whether an exclusive writer currently owns the resource.
            bool _writerOwned = false;


            // Internal acquisition helpers.

            /// Takes one native mutex using the remaining wait budget.
            bool Take(
                SemaphoreHandle_t handle,
                ESPressio::Platform::FreeRTOS::Detail::WaitBudget& budget
            ) noexcept {
                return budget.Take(
                    handle
                );
            }

        public:

            // Construction and lifetime.

            /// Creates an unlocked FreeRTOS read/write mutex.
            ReadWriteMutexProvider() noexcept :
                _readerGuard(
                    xSemaphoreCreateMutexStatic(
                        &_readerGuardStorage
                    )
                ),
                _turnstile(
                    xSemaphoreCreateMutexStatic(
                        &_turnstileStorage
                    )
                ),
                _resource(
                    xSemaphoreCreateBinaryStatic(
                        &_resourceStorage
                    )
                ) {
                if (_resource != nullptr) {
                    (void)xSemaphoreGive(
                        _resource
                    );
                }
            }

            /// Prevents copying read/write lock state.
            ReadWriteMutexProvider(const ReadWriteMutexProvider&) = delete;

            /// Prevents copy assignment of read/write lock state.
            ReadWriteMutexProvider& operator =(const ReadWriteMutexProvider&) = delete;

            /// Prevents moving read/write lock state.
            ReadWriteMutexProvider(ReadWriteMutexProvider&&) = delete;

            /// Prevents move assignment of read/write lock state.
            ReadWriteMutexProvider& operator =(ReadWriteMutexProvider&&) = delete;


            // Shared-read operations.

            /// Acquires one shared read lock while respecting an already-waiting writer.
            ESPressio::Platform::Synchronization::LockAcquireResult AcquireRead(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (
                    _readerGuard == nullptr ||
                    _turnstile == nullptr ||
                    _resource == nullptr
                ) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::ProviderFailure;
                }

                ESPressio::Platform::FreeRTOS::Detail::WaitBudget budget(timeout);

                if (!Take(
                    _turnstile,
                    budget
                )) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
                }

                (void)xSemaphoreGive(
                    _turnstile
                );

                if (!Take(
                    _readerGuard,
                    budget
                )) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
                }

                if (
                    _readers == 0U &&
                    !Take(
                        _resource,
                        budget
                    )
                ) {
                    (void)xSemaphoreGive(
                        _readerGuard
                    );
                    return ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
                }

                ++_readers;
                (void)xSemaphoreGive(
                    _readerGuard
                );

                return ESPressio::Platform::Synchronization::LockAcquireResult::Acquired;
            }

            /// Releases one shared read lock.
            ESPressio::Platform::Synchronization::LockReleaseResult ReleaseRead() noexcept {
                if (_readerGuard == nullptr || _resource == nullptr) {
                    return ESPressio::Platform::Synchronization::LockReleaseResult::ProviderFailure;
                }

                if (
                    xSemaphoreTake(
                        _readerGuard,
                        portMAX_DELAY
                    ) != pdTRUE
                ) {
                    return ESPressio::Platform::Synchronization::LockReleaseResult::ProviderFailure;
                }

                if (_readers == 0U) {
                    (void)xSemaphoreGive(
                        _readerGuard
                    );
                    return ESPressio::Platform::Synchronization::LockReleaseResult::InvalidOwnership;
                }

                --_readers;

                if (_readers == 0U) {
                    (void)xSemaphoreGive(
                        _resource
                    );
                }

                (void)xSemaphoreGive(
                    _readerGuard
                );

                return ESPressio::Platform::Synchronization::LockReleaseResult::Released;
            }


            // Exclusive-write operations.

            /// Acquires the exclusive writer lock and blocks new readers behind the turnstile.
            ESPressio::Platform::Synchronization::LockAcquireResult AcquireWrite(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (_turnstile == nullptr || _resource == nullptr) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::ProviderFailure;
                }

                ESPressio::Platform::FreeRTOS::Detail::WaitBudget budget(timeout);

                if (!Take(
                    _turnstile,
                    budget
                )) {
                    return ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
                }

                if (!Take(
                    _resource,
                    budget
                )) {
                    (void)xSemaphoreGive(
                        _turnstile
                    );
                    return ESPressio::Platform::Synchronization::LockAcquireResult::TimedOut;
                }

                _writerOwned = true;
                return ESPressio::Platform::Synchronization::LockAcquireResult::Acquired;
            }

            /// Releases the exclusive writer lock and reopens the reader/writer turnstile.
            ESPressio::Platform::Synchronization::LockReleaseResult ReleaseWrite() noexcept {
                if (
                    _turnstile == nullptr ||
                    _resource == nullptr
                ) {
                    return ESPressio::Platform::Synchronization::LockReleaseResult::ProviderFailure;
                }

                if (!_writerOwned) {
                    return ESPressio::Platform::Synchronization::LockReleaseResult::InvalidOwnership;
                }

                _writerOwned = false;

                const auto resourceReleased = xSemaphoreGive(
                    _resource
                );

                const auto turnstileReleased = xSemaphoreGive(
                    _turnstile
                );

                return resourceReleased == pdTRUE && turnstileReleased == pdTRUE
                    ? ESPressio::Platform::Synchronization::LockReleaseResult::Released
                    : ESPressio::Platform::Synchronization::LockReleaseResult::ProviderFailure;
            }

    };


    /// Compile-time validation of the FreeRTOS ReadWriteMutex provider.
    using ReadWriteMutexContract = ESPressio::Platform::Synchronization::Detail::ReadWriteMutexProviderTraits<ReadWriteMutexProvider>;

#endif


#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )

    /// FreeRTOS static counting-semaphore provider.
    class CountingSemaphoreProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                ESPressio::Platform::Synchronization::CountingSemaphore,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::CountingSemaphoreWaitResolutionNanoseconds,
                    ESPressio::Platform::FreeRTOS::Detail::WaitResolutionNanoseconds
                >,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::CountingSemaphoreSupportsInterruptRelease,
                    true
                >
            >
        >
    > {

        private:

            // Native synchronization state.

            /// Caller-owned FreeRTOS counting-semaphore storage.
            StaticSemaphore_t _storage{};

            /// Native counting-semaphore handle after initialization.
            SemaphoreHandle_t _handle = nullptr;

        public:

            // Construction and lifetime.

            /// Creates an uninitialized counting semaphore.
            CountingSemaphoreProvider() noexcept = default;

            /// Prevents copying semaphore state.
            CountingSemaphoreProvider(const CountingSemaphoreProvider&) = delete;

            /// Prevents copy assignment of semaphore state.
            CountingSemaphoreProvider& operator =(const CountingSemaphoreProvider&) = delete;

            /// Prevents moving semaphore state.
            CountingSemaphoreProvider(CountingSemaphoreProvider&&) = delete;

            /// Prevents move assignment of semaphore state.
            CountingSemaphoreProvider& operator =(CountingSemaphoreProvider&&) = delete;


            // Initialization.

            /// Initializes the static counting semaphore with fixed counts.
            ESPressio::Platform::Synchronization::SemaphoreInitializationResult Initialize(
                std::uint32_t maximum,
                std::uint32_t initial
            ) noexcept {
                if (_handle != nullptr) {
                    return ESPressio::Platform::Synchronization::SemaphoreInitializationResult::AlreadyInitialized;
                }

                if (maximum == 0U) {
                    return ESPressio::Platform::Synchronization::SemaphoreInitializationResult::InvalidMaximumCount;
                }

                if (initial > maximum) {
                    return ESPressio::Platform::Synchronization::SemaphoreInitializationResult::InvalidInitialCount;
                }

                _handle = xSemaphoreCreateCountingStatic(
                    static_cast<UBaseType_t>(maximum),
                    static_cast<UBaseType_t>(initial),
                    &_storage
                );

                return _handle != nullptr
                    ? ESPressio::Platform::Synchronization::SemaphoreInitializationResult::Succeeded
                    : ESPressio::Platform::Synchronization::SemaphoreInitializationResult::ProviderFailure;
            }


            // Permit operations.

            /// Acquires one permit according to the requested wait policy.
            ESPressio::Platform::Synchronization::SemaphoreAcquireResult Acquire(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::SemaphoreAcquireResult::NotInitialized;
                }

                ESPressio::Platform::FreeRTOS::Detail::WaitBudget budget(timeout);

                return budget.Take(
                    _handle
                )
                    ? ESPressio::Platform::Synchronization::SemaphoreAcquireResult::Acquired
                    : ESPressio::Platform::Synchronization::SemaphoreAcquireResult::TimedOut;
            }

            /// Releases one permit.
            ESPressio::Platform::Synchronization::SemaphoreReleaseResult Release() noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::SemaphoreReleaseResult::NotInitialized;
                }

                return xSemaphoreGive(
                    _handle
                ) == pdTRUE
                    ? ESPressio::Platform::Synchronization::SemaphoreReleaseResult::Released
                    : ESPressio::Platform::Synchronization::SemaphoreReleaseResult::CapacityReached;
            }

            /// Releases one permit from interrupt context.
            ESPressio::Platform::Synchronization::SemaphoreReleaseResult ReleaseFromInterrupt() noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::SemaphoreReleaseResult::NotInitialized;
                }

                BaseType_t higherPriorityTaskWoken = pdFALSE;
                const auto result = xSemaphoreGiveFromISR(
                    _handle,
                    &higherPriorityTaskWoken
                );

                return result == pdTRUE
                    ? ESPressio::Platform::Synchronization::SemaphoreReleaseResult::Released
                    : ESPressio::Platform::Synchronization::SemaphoreReleaseResult::CapacityReached;
            }

    };


    /// Compile-time validation of the FreeRTOS CountingSemaphore provider.
    using CountingSemaphoreContract = ESPressio::Platform::Synchronization::Detail::CountingSemaphoreProviderTraits<CountingSemaphoreProvider>;

#endif


#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( INCLUDE_vTaskSuspend == 1 )

    /// FreeRTOS static binary-semaphore-backed latched signal provider.
    class SignalProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
        Framework::Offers<
            Framework::Offer<
                ESPressio::Platform::Synchronization::Signal,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::SignalWaitResolutionNanoseconds,
                    ESPressio::Platform::FreeRTOS::Detail::WaitResolutionNanoseconds
                >,
                Framework::PropertyValue<
                    ESPressio::Platform::Synchronization::SignalSupportsInterruptNotification,
                    true
                >
            >
        >
    > {

        private:

            // Native synchronization state.

            /// Caller-owned FreeRTOS binary-semaphore storage.
            StaticSemaphore_t _storage{};

            /// Native binary-semaphore handle.
            SemaphoreHandle_t _handle;

        public:

            // Construction and lifetime.

            /// Creates an unsignaled static FreeRTOS signal.
            SignalProvider() noexcept :
                _handle(
                    xSemaphoreCreateBinaryStatic(
                        &_storage
                    )
                ) {}

            /// Prevents copying signal state.
            SignalProvider(const SignalProvider&) = delete;

            /// Prevents copy assignment of signal state.
            SignalProvider& operator =(const SignalProvider&) = delete;

            /// Prevents moving signal state.
            SignalProvider(SignalProvider&&) = delete;

            /// Prevents move assignment of signal state.
            SignalProvider& operator =(SignalProvider&&) = delete;


            // Notification.

            /// Latches the signal; repeated notifications coalesce while already latched.
            ESPressio::Platform::Synchronization::SignalNotifyResult Notify() noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::SignalNotifyResult::ProviderFailure;
                }

                (void)xSemaphoreGive(
                    _handle
                );

                // A binary semaphore that is already available is already latched, which is
                // semantically the same successful Signal state for ESPressio.
                return ESPressio::Platform::Synchronization::SignalNotifyResult::Signaled;
            }

            /// Latches the signal from interrupt context.
            ESPressio::Platform::Synchronization::SignalNotifyResult NotifyFromInterrupt() noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::SignalNotifyResult::ProviderFailure;
                }

                BaseType_t higherPriorityTaskWoken = pdFALSE;
                const auto result = xSemaphoreGiveFromISR(
                    _handle,
                    &higherPriorityTaskWoken
                );

                // pdFAIL is also a valid coalesced state for a binary semaphore: it means
                // the latch was already available before this notification.
                (void)result;
                return ESPressio::Platform::Synchronization::SignalNotifyResult::Signaled;
            }


            // Waiting.

            /// Waits for and consumes the current signal latch.
            ESPressio::Platform::Synchronization::SignalWaitResult Wait(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept {
                if (_handle == nullptr) {
                    return ESPressio::Platform::Synchronization::SignalWaitResult::ProviderFailure;
                }

                ESPressio::Platform::FreeRTOS::Detail::WaitBudget budget(timeout);

                return budget.Take(
                    _handle
                )
                    ? ESPressio::Platform::Synchronization::SignalWaitResult::Signaled
                    : ESPressio::Platform::Synchronization::SignalWaitResult::TimedOut;
            }

    };


    /// Compile-time validation of the FreeRTOS Signal provider.
    using SignalContract = ESPressio::Platform::Synchronization::Detail::SignalProviderTraits<SignalProvider>;

#endif

} // ESPressio::Platform::FreeRTOS::Synchronization
