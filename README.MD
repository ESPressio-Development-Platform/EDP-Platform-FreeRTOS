# ESPressio Platform FreeRTOS

`EDP-Platform-FreeRTOS` provides concrete `EDP-Platform` implementations backed specifically by portable FreeRTOS APIs.

## Providers

Where the corresponding FreeRTOS configuration enables the required static primitive and indefinite blocking semantics (`INCLUDE_vTaskSuspend == 1`), this repository provides:

```text
ExecutionContext
Mutex
RecursiveMutex
ReadWriteMutex
CountingSemaphore
Signal
```

The `ReadWriteMutex` provider is assembled from static FreeRTOS mutex primitives and uses a writer turnstile to prevent an already-waiting writer from being bypassed indefinitely by new readers.

`SpinLock` is not provided here because vanilla FreeRTOS does not define one portable cross-port spin-lock API.

`AtomicWord32` is no longer owned by this repository because its implementation used only Standard C++ `std::atomic`. That provider now belongs to `EDP-Platform-Portable`.

## Static Resource Policy

The execution provider uses `xTaskCreateStatic`.

The caller supplies:

```text
StaticTask_t-compatible control storage
StackType_t-compatible stack storage
```

No FreeRTOS heap allocation is used for the native task.

The task begins behind an internal start gate. After the user entry function returns it signals completion and suspends until the owner calls `Destroy()`.

`Join()` is the ownership synchronization point after a started entry returns. `Destroy()` refuses to forcibly delete a running or unjoined execution scope.

When `uxTaskGetStackHighWaterMark()` is enabled by the FreeRTOS configuration, the provider reports minimum free stack storage normalized to bytes.

## Waits

ESPressio finite waits are expressed in nanoseconds.

The provider rounds them upward to FreeRTOS ticks so unit conversion never causes an earlier timeout.

## Interrupt Operations

The counting semaphore and signal providers expose FreeRTOS ISR-safe release/notification paths.

## Dependencies

```text
EDP-Platform-FreeRTOS
└── EDP-Platform [mandatory]
    └── EDP-System [transitive]
```

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD).
