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

        return 0;
    }

} // Test
