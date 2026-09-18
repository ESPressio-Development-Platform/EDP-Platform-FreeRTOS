#include <ESPressio_Platform_FreeRTOS.hpp>

namespace Demo {

    /// Exercises the FreeRTOS synchronization providers in one deterministic scope.
    int Run() noexcept {
        using namespace ESPressio::Platform;

        FreeRTOS::Synchronization::MutexProvider mutex;

        if (
            mutex.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) != Synchronization::LockAcquireResult::Acquired
        ) {
            return 1;
        }

        if (
            mutex.Release() !=
            Synchronization::LockReleaseResult::Released
        ) {
            return 2;
        }

        FreeRTOS::Synchronization::RecursiveMutexProvider recursiveMutex;

        if (
            recursiveMutex.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) != Synchronization::LockAcquireResult::Acquired
        ) {
            return 3;
        }

        if (
            recursiveMutex.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) != Synchronization::LockAcquireResult::Acquired
        ) {
            return 4;
        }

        if (
            recursiveMutex.Release() !=
            Synchronization::LockReleaseResult::Released
        ) {
            return 5;
        }

        if (
            recursiveMutex.Release() !=
            Synchronization::LockReleaseResult::Released
        ) {
            return 6;
        }

        FreeRTOS::Synchronization::ReadWriteMutexProvider readWriteMutex;

        if (
            readWriteMutex.AcquireRead(
                Synchronization::WaitTimeout::NoWait()
            ) != Synchronization::LockAcquireResult::Acquired
        ) {
            return 7;
        }

        if (
            readWriteMutex.ReleaseRead() !=
            Synchronization::LockReleaseResult::Released
        ) {
            return 8;
        }

        if (
            readWriteMutex.AcquireWrite(
                Synchronization::WaitTimeout::NoWait()
            ) != Synchronization::LockAcquireResult::Acquired
        ) {
            return 9;
        }

        if (
            readWriteMutex.ReleaseWrite() !=
            Synchronization::LockReleaseResult::Released
        ) {
            return 10;
        }

        FreeRTOS::Synchronization::CountingSemaphoreProvider semaphore;

        if (
            semaphore.Initialize(
                2U,
                1U
            ) != Synchronization::SemaphoreInitializationResult::Succeeded
        ) {
            return 11;
        }

        if (
            semaphore.Acquire(
                Synchronization::WaitTimeout::NoWait()
            ) != Synchronization::SemaphoreAcquireResult::Acquired
        ) {
            return 12;
        }

        if (
            semaphore.Release() !=
            Synchronization::SemaphoreReleaseResult::Released
        ) {
            return 13;
        }

        FreeRTOS::Synchronization::SignalProvider signal;

        if (
            signal.Notify() !=
            Synchronization::SignalNotifyResult::Signaled
        ) {
            return 14;
        }

        return signal.Wait(
            Synchronization::WaitTimeout::NoWait()
        ) == Synchronization::SignalWaitResult::Signaled
            ? 0
            : 15;
    }

} // Demo


/// Runs the FreeRTOS synchronization demonstration once.
extern "C" void app_main() {
    static_cast<void>(
        Demo::Run()
    );
}
