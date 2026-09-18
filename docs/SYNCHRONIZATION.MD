# FreeRTOS Synchronization Providers

## Purpose and scope

`EDP-Platform-FreeRTOS` supplies concrete implementations of portable `EDP-Platform` synchronization capabilities using FreeRTOS static semaphore and mutex APIs.

The currently implemented capability family is:

```text
Mutex
RecursiveMutex
ReadWriteMutex
CountingSemaphore
Signal
```

`SpinLock` is intentionally absent because vanilla FreeRTOS does not expose one portable cross-port spin-lock contract.

## Provider availability

Provider declarations are guarded by the FreeRTOS features they require.

### Mutex

Available when:

```text
configSUPPORT_STATIC_ALLOCATION == 1
configUSE_MUTEXES == 1
INCLUDE_vTaskSuspend == 1
```

### RecursiveMutex

Available when:

```text
configSUPPORT_STATIC_ALLOCATION == 1
configUSE_RECURSIVE_MUTEXES == 1
INCLUDE_vTaskSuspend == 1
```

### ReadWriteMutex

Available when:

```text
configSUPPORT_STATIC_ALLOCATION == 1
configUSE_MUTEXES == 1
INCLUDE_vTaskSuspend == 1
```

### CountingSemaphore

Available when:

```text
configSUPPORT_STATIC_ALLOCATION == 1
configUSE_COUNTING_SEMAPHORES == 1
INCLUDE_vTaskSuspend == 1
```

### Signal

Available when:

```text
configSUPPORT_STATIC_ALLOCATION == 1
INCLUDE_vTaskSuspend == 1
```

These guards mean a consuming Composition can only name providers that the selected FreeRTOS configuration can actually build.

## Static resource model

The providers use embedded `StaticSemaphore_t` storage rather than dynamically allocated FreeRTOS synchronization objects.

Mutex, RecursiveMutex, CountingSemaphore, and Signal each retain their required native static storage inside the provider object.

`ReadWriteMutex` retains three static native synchronization objects:

```text
reader-count guard mutex
writer turnstile mutex
shared-resource binary semaphore
```

No hidden worker task, callback registry, or heap-owned synchronization object is introduced by this provider family.

## Wait representation and resolution

Blocking operations consume the Platform `WaitTimeout` abstraction:

```text
NoWait
finite relative nanoseconds
WaitForever
```

The provider advertises a wait resolution of:

```text
ceil(1,000,000,000 / configTICK_RATE_HZ) nanoseconds
```

Finite waits are rounded upward to native ticks. Conversion therefore does not intentionally make a requested wait expire early.

The internal wait budget tracks one total finite timeout across multi-stage native operations. This matters particularly for `ReadWriteMutex`, whose acquisition can touch more than one native primitive. The original timeout is not restarted for each internal step.

Very long finite waits are consumed in chunks below the native `portMAX_DELAY` sentinel where necessary.

## Mutex

`MutexProvider` wraps one static FreeRTOS mutex.

`Acquire(timeout)` reports `Acquired`, `TimedOut`, or `ProviderFailure` according to the Platform contract.

`Release()` maps native give failure to `InvalidOwnership`, preserving the normal mutex ownership rule that the releasing execution context must own the mutex.

## RecursiveMutex

`RecursiveMutexProvider` wraps one static FreeRTOS recursive mutex.

Repeated acquisition by the owning execution context is supported by the native recursive-mutex API. Each successful acquisition requires a corresponding release.

Release by an invalid owner is reported through the Platform `LockReleaseResult` contract.

## ReadWriteMutex

FreeRTOS does not provide the required reader/writer abstraction directly, so the provider assembles it from static primitives.

The writer path first acquires a turnstile and then the shared resource. Once a writer owns the turnstile, new readers cannot bypass that already-waiting writer indefinitely.

Readers briefly pass through the same turnstile, then update an aggregate reader count under the reader guard. The first active reader takes the shared-resource binary semaphore; the final reader releases it.

The shared resource is a binary semaphore rather than a mutex because ownership may legitimately pass across distinct reader tasks as the first reader acquires it and the final reader releases it.

The provider tracks aggregate reader count and writer ownership. It does not retain a per-task reader ownership registry.

## CountingSemaphore

`CountingSemaphoreProvider` is explicitly initialized with:

```text
maximum permit count
initial permit count
```

Initialization rejects:

```text
maximum == 0
initial > maximum
re-initialization of an already initialized provider
```

`Acquire()` consumes one permit according to the requested wait policy.

`Release()` reports `CapacityReached` rather than overflowing the configured maximum.

The provider advertises interrupt-context release support and implements `ReleaseFromInterrupt()` through `xSemaphoreGiveFromISR()`.

## Signal

`SignalProvider` implements the Platform latched/coalescing `Signal` contract with a static FreeRTOS binary semaphore.

`Notify()` latches readiness. Repeated notifications while already latched intentionally coalesce into the same successful signaled state.

`Wait()` consumes the current latch on success.

The provider advertises interrupt-context notification support and maps `NotifyFromInterrupt()` to `xSemaphoreGiveFromISR()`.

A native binary-semaphore give failure caused solely by an already-available latch is still treated as a successful ESPressio signal notification because the required logical state is already established.

## Interrupt-context boundary

Only operations explicitly advertised by the Platform capability may be invoked from interrupt context.

Current FreeRTOS interrupt support is:

```text
CountingSemaphore ReleaseFromInterrupt   supported
Signal NotifyFromInterrupt               supported
Mutex operations                         not exposed as interrupt operations
RecursiveMutex operations                not exposed as interrupt operations
ReadWriteMutex operations                not exposed as interrupt operations
```

The provider performs the FreeRTOS ISR-safe give operation. Higher scheduling or ISR-exit policy remains outside the synchronization abstraction.

## Failure and ownership semantics

Operations use the strongly typed result enums defined by `EDP-Platform`.

Provider construction or native-handle failure is surfaced as `ProviderFailure` where the contract permits it.

Timeouts remain distinct from provider failures, capacity exhaustion, initialization failure, and invalid ownership.

## Integration boundary

These providers implement the abstractions defined by [`EDP-Platform`](https://github.com/ESPressio-Development-Platform/EDP-Platform) but repository-local documentation links remain relative within this repository.

Provider ownership follows the backing API. An Arduino-ESP32 application may select these providers even though the application itself is built through Arduino, because the implementation is FreeRTOS-backed rather than Arduino-backed.
