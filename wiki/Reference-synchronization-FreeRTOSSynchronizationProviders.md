# src/synchronization/FreeRTOSSynchronizationProviders.hpp

**Primary classification:** PUBLIC PROVIDER / EXTENSION API

**Source baseline:** `85baafa13f13d7d647e7a2519c17f989c72b4f15`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Platform-FreeRTOS/blob/85baafa13f13d7d647e7a2519c17f989c72b4f15/src/synchronization/FreeRTOSSynchronizationProviders.hpp)

## Direct includes

- `cstdint`
- `../detail/FreeRTOSHeaders.hpp`
- `ESPressio_Platform.hpp`
- `../detail/FreeRTOSWait.hpp`

## Documented declarations

### `MutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

FreeRTOS static-mutex provider.

```cpp
class MutexProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
```

### `StaticSemaphore_t _storage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Caller-owned FreeRTOS mutex storage embedded in this provider object.

```cpp
StaticSemaphore_t _storage{};
```

### `_handle`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Native FreeRTOS mutex handle.

```cpp
SemaphoreHandle_t _handle;
```

### `MutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Creates an unlocked static FreeRTOS mutex.

```cpp
MutexProvider() noexcept :
                _handle(
                    xSemaphoreCreateMutexStatic(
                        &_storage
                    )
                ) {}
```

### `MutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copying mutex state.

```cpp
MutexProvider(const MutexProvider&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copy assignment of mutex state.

```cpp
MutexProvider& operator =(const MutexProvider&) = delete;
```

### `MutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents moving mutex state.

```cpp
MutexProvider(MutexProvider&&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents move assignment of mutex state.

```cpp
MutexProvider& operator =(MutexProvider&&) = delete;
```

### `Acquire`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Acquires the mutex according to the requested wait policy.

```cpp
ESPressio::Platform::Synchronization::LockAcquireResult Acquire(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept
```

### `Release`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Releases the mutex owned by the current execution context.

```cpp
ESPressio::Platform::Synchronization::LockReleaseResult Release() noexcept
```

### `MutexContract`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Compile-time validation of the FreeRTOS Mutex provider.

```cpp
using MutexContract = ESPressio::Platform::Synchronization::Detail::MutexProviderTraits<MutexProvider>;
```

### `RecursiveMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

FreeRTOS static recursive-mutex provider.

```cpp
class RecursiveMutexProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
```

### `StaticSemaphore_t _storage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Caller-owned FreeRTOS recursive-mutex storage embedded in this provider object.

```cpp
StaticSemaphore_t _storage{};
```

### `_handle`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Native FreeRTOS recursive-mutex handle.

```cpp
SemaphoreHandle_t _handle;
```

### `RecursiveMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Creates an unlocked static FreeRTOS recursive mutex.

```cpp
RecursiveMutexProvider() noexcept :
                _handle(
                    xSemaphoreCreateRecursiveMutexStatic(
                        &_storage
                    )
                ) {}
```

### `RecursiveMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copying mutex state.

```cpp
RecursiveMutexProvider(const RecursiveMutexProvider&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copy assignment of mutex state.

```cpp
RecursiveMutexProvider& operator =(const RecursiveMutexProvider&) = delete;
```

### `RecursiveMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents moving mutex state.

```cpp
RecursiveMutexProvider(RecursiveMutexProvider&&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents move assignment of mutex state.

```cpp
RecursiveMutexProvider& operator =(RecursiveMutexProvider&&) = delete;
```

### `Acquire`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Acquires the recursive mutex according to the requested wait policy.

```cpp
ESPressio::Platform::Synchronization::LockAcquireResult Acquire(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept
```

### `Release`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Releases one recursive acquisition owned by the current execution context.

```cpp
ESPressio::Platform::Synchronization::LockReleaseResult Release() noexcept
```

### `RecursiveMutexContract`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Compile-time validation of the FreeRTOS RecursiveMutex provider.

```cpp
using RecursiveMutexContract = ESPressio::Platform::Synchronization::Detail::RecursiveMutexProviderTraits<RecursiveMutexProvider>;
```

### `ReadWriteMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

FreeRTOS reader/writer lock assembled from static mutex primitives.

New readers pass through a writer turnstile so a waiting writer can prevent reader starvation.

```cpp
class ReadWriteMutexProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
```

### `StaticSemaphore_t _readerGuardStorage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Protects the reader-count transition.

```cpp
StaticSemaphore_t _readerGuardStorage{};
```

### `StaticSemaphore_t _turnstileStorage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Serializes writers and gates new readers behind waiting writers.

```cpp
StaticSemaphore_t _turnstileStorage{};
```

### `StaticSemaphore_t _resourceStorage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Stores the binary semaphore excluding writers while readers are active.

```cpp
StaticSemaphore_t _resourceStorage{};
```

### `_readerGuard`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Reader-count mutex.

```cpp
SemaphoreHandle_t _readerGuard;
```

### `_turnstile`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Writer turnstile mutex.

```cpp
SemaphoreHandle_t _turnstile;
```

### `_resource`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Shared resource binary semaphore; ownership may pass between distinct reader tasks.

```cpp
SemaphoreHandle_t _resource;
```

### `_readers`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Number of active shared readers.

```cpp
std::uint32_t _readers = 0U;
```

### `_writerOwned`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Indicates whether an exclusive writer currently owns the resource.

```cpp
bool _writerOwned = false;
```

### `Take`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Takes one native mutex using the remaining wait budget.

```cpp
bool Take(
                SemaphoreHandle_t handle,
                ESPressio::Platform::FreeRTOS::Detail::WaitBudget& budget
            ) noexcept
```

### `ReadWriteMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Creates an unlocked FreeRTOS read/write mutex.

```cpp
ReadWriteMutexProvider() noexcept :
                _readerGuard(
                    xSemaphoreCreateMutexStatic(
                        &_readerGuardStorage
                    )
                ),
```

### `ReadWriteMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copying read/write lock state.

```cpp
ReadWriteMutexProvider(const ReadWriteMutexProvider&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copy assignment of read/write lock state.

```cpp
ReadWriteMutexProvider& operator =(const ReadWriteMutexProvider&) = delete;
```

### `ReadWriteMutexProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents moving read/write lock state.

```cpp
ReadWriteMutexProvider(ReadWriteMutexProvider&&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents move assignment of read/write lock state.

```cpp
ReadWriteMutexProvider& operator =(ReadWriteMutexProvider&&) = delete;
```

### `AcquireRead`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Acquires one shared read lock while respecting an already-waiting writer.

```cpp
ESPressio::Platform::Synchronization::LockAcquireResult AcquireRead(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept
```

### `ReleaseRead`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Releases one shared read lock.

```cpp
ESPressio::Platform::Synchronization::LockReleaseResult ReleaseRead() noexcept
```

### `AcquireWrite`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Acquires the exclusive writer lock and blocks new readers behind the turnstile.

```cpp
ESPressio::Platform::Synchronization::LockAcquireResult AcquireWrite(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept
```

### `ReleaseWrite`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Releases the exclusive writer lock and reopens the reader/writer turnstile.

```cpp
ESPressio::Platform::Synchronization::LockReleaseResult ReleaseWrite() noexcept
```

### `ReadWriteMutexContract`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Compile-time validation of the FreeRTOS ReadWriteMutex provider.

```cpp
using ReadWriteMutexContract = ESPressio::Platform::Synchronization::Detail::ReadWriteMutexProviderTraits<ReadWriteMutexProvider>;
```

### `CountingSemaphoreProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

FreeRTOS static counting-semaphore provider.

```cpp
class CountingSemaphoreProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
```

### `StaticSemaphore_t _storage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Caller-owned FreeRTOS counting-semaphore storage.

```cpp
StaticSemaphore_t _storage{};
```

### `_handle`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Native counting-semaphore handle after initialization.

```cpp
SemaphoreHandle_t _handle = nullptr;
```

### `CountingSemaphoreProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Creates an uninitialized counting semaphore.

```cpp
CountingSemaphoreProvider() noexcept = default;
```

### `CountingSemaphoreProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copying semaphore state.

```cpp
CountingSemaphoreProvider(const CountingSemaphoreProvider&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copy assignment of semaphore state.

```cpp
CountingSemaphoreProvider& operator =(const CountingSemaphoreProvider&) = delete;
```

### `CountingSemaphoreProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents moving semaphore state.

```cpp
CountingSemaphoreProvider(CountingSemaphoreProvider&&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents move assignment of semaphore state.

```cpp
CountingSemaphoreProvider& operator =(CountingSemaphoreProvider&&) = delete;
```

### `Initialize`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Initializes the static counting semaphore with fixed counts.

```cpp
ESPressio::Platform::Synchronization::SemaphoreInitializationResult Initialize(
                std::uint32_t maximum,
                std::uint32_t initial
            ) noexcept
```

### `Acquire`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Acquires one permit according to the requested wait policy.

```cpp
ESPressio::Platform::Synchronization::SemaphoreAcquireResult Acquire(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept
```

### `Release`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Releases one permit.

```cpp
ESPressio::Platform::Synchronization::SemaphoreReleaseResult Release() noexcept
```

### `ReleaseFromInterrupt`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Releases one permit from interrupt context.

```cpp
ESPressio::Platform::Synchronization::SemaphoreReleaseResult ReleaseFromInterrupt() noexcept
```

### `CountingSemaphoreContract`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Compile-time validation of the FreeRTOS CountingSemaphore provider.

```cpp
using CountingSemaphoreContract = ESPressio::Platform::Synchronization::Detail::CountingSemaphoreProviderTraits<CountingSemaphoreProvider>;
```

### `SignalProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API

FreeRTOS static binary-semaphore-backed latched signal provider.

```cpp
class SignalProvider final : public Framework::Provider<
        ESPressio::Platform::Domain,
```

### `StaticSemaphore_t _storage{};`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Caller-owned FreeRTOS binary-semaphore storage.

```cpp
StaticSemaphore_t _storage{};
```

### `_handle`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Native binary-semaphore handle.

```cpp
SemaphoreHandle_t _handle;
```

### `SignalProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Creates an unsignaled static FreeRTOS signal.

```cpp
SignalProvider() noexcept :
                _handle(
                    xSemaphoreCreateBinaryStatic(
                        &_storage
                    )
                ) {}
```

### `SignalProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copying signal state.

```cpp
SignalProvider(const SignalProvider&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents copy assignment of signal state.

```cpp
SignalProvider& operator =(const SignalProvider&) = delete;
```

### `SignalProvider`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents moving signal state.

```cpp
SignalProvider(SignalProvider&&) = delete;
```

### `operator`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Prevents move assignment of signal state.

```cpp
SignalProvider& operator =(SignalProvider&&) = delete;
```

### `Notify`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Latches the signal; repeated notifications coalesce while already latched.

```cpp
ESPressio::Platform::Synchronization::SignalNotifyResult Notify() noexcept
```

### `NotifyFromInterrupt`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Latches the signal from interrupt context.

```cpp
ESPressio::Platform::Synchronization::SignalNotifyResult NotifyFromInterrupt() noexcept
```

### `Wait`

**Classification:** PUBLIC PROVIDER / EXTENSION API · source access: `public`

Waits for and consumes the current signal latch.

```cpp
ESPressio::Platform::Synchronization::SignalWaitResult Wait(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept
```

### `SignalContract`

**Classification:** PUBLIC PROVIDER / EXTENSION API

Compile-time validation of the FreeRTOS Signal provider.

```cpp
using SignalContract = ESPressio::Platform::Synchronization::Detail::SignalProviderTraits<SignalProvider>;
```

## Preprocessor controls in this header

- `#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )`
- `#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_RECURSIVE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )`
- `#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_MUTEXES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )`
- `#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configUSE_COUNTING_SEMAPHORES == 1 ) && ( INCLUDE_vTaskSuspend == 1 )`
- `#if ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( INCLUDE_vTaskSuspend == 1 )`
