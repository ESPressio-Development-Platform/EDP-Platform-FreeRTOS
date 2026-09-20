#include <cassert>

#include <ESPressio_Platform_FreeRTOS.hpp>

namespace Test {

    /// Exercises the provider surfaces available under the selected FreeRTOS configuration.
    int Run() noexcept {
        using namespace ESPressio::Platform;

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )
        FreeRTOS::Synchronization::MutexProvider mutex;

        assert(
            mutex.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) == Synchronization::LockAcquireResult::Acquired
        );

        assert(
            mutex.Release() ==
            Synchronization::LockReleaseResult::Released
        );

        FreeRTOS::Synchronization::ReadWriteMutexProvider readWriteMutex;

        assert(
            readWriteMutex.AcquireRead(
                Synchronization::WaitTimeout::NoWait()
            ) == Synchronization::LockAcquireResult::Acquired
        );

        assert(
            readWriteMutex.ReleaseRead() ==
            Synchronization::LockReleaseResult::Released
        );
#endif

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_RECURSIVE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )
        FreeRTOS::Synchronization::RecursiveMutexProvider recursiveMutex;

        assert(
            recursiveMutex.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) == Synchronization::LockAcquireResult::Acquired
        );

        assert(
            recursiveMutex.Release() ==
            Synchronization::LockReleaseResult::Released
        );
#endif

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )
        FreeRTOS::Synchronization::CountingSemaphoreProvider semaphore;

        assert(
            semaphore.Initialize(
                1U,
                1U
            ) == Synchronization::SemaphoreInitializationResult::Succeeded
        );

        assert(
            semaphore.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) == Synchronization::SemaphoreAcquireResult::Acquired
        );

        assert(
            semaphore.Release() ==
            Synchronization::SemaphoreReleaseResult::Released
        );
#endif

#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( INCLUDE_vTaskSuspend == 1 )
        FreeRTOS::Synchronization::SignalProvider signal;

        assert(
            signal.Notify() ==
            Synchronization::SignalNotifyResult::Signaled
        );

        assert(
            signal.Wait(
                Synchronization::WaitTimeout::NoWait()
            ) == Synchronization::SignalWaitResult::Signaled
        );
#endif


#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( INCLUDE_vTaskDelete == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) && ( INCLUDE_xTaskGetCurrentTaskHandle == 1 )
        using ExecutionProvider = FreeRTOS::Execution::ExecutionContextProvider;
        using ExecutionContract = Execution::Detail::ExecutionContextProviderTraits<ExecutionProvider>;

        static_assert(
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::Low
            ) <=
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::Normal
            ),
            "Native priority mapping must preserve logical ordering from Low to Normal"
        );

        static_assert(
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::Normal
            ) <=
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::High
            ),
            "Native priority mapping must preserve logical ordering from Normal to High"
        );

        static_assert(
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::High
            ) <=
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::Critical
            ),
            "Native priority mapping must preserve logical ordering from High to Critical"
        );

        static_assert(
            ESPressio::Platform::FreeRTOS::Execution::Detail::NativePriorityFor(
                Execution::ExecutionPriority::Critical
            ) ==
            static_cast<UBaseType_t>(
                configMAX_PRIORITIES - 1U
            ),
            "Critical must map to the highest configured native priority"
        );

        static_assert(
            ExecutionContract::Properties::template Value<
                Execution::CallerSuppliedStorage
            >,
            "FreeRTOS execution provider must use caller-supplied storage"
        );

        static_assert(
            !ExecutionContract::Properties::template Value<
                Execution::SupportsProcessorAffinity
            >,
            "Vanilla FreeRTOS execution provider must not claim processor affinity"
        );
#endif

        return 0;
    }

} // Test
