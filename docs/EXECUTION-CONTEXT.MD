# FreeRTOS ExecutionContext Provider

## Purpose and scope

`EDP-Platform-FreeRTOS` supplies the portable FreeRTOS concrete provider for the `EDP-Platform` `ExecutionContext` capability.

It maps the Platform execution contract onto the vanilla FreeRTOS static-task API. Higher ESPressio Task, Thread, pooling, loop, cancellation, and scheduler semantics remain outside this repository.

## Availability

The provider is compiled only when the FreeRTOS configuration exposes every native facility required by the current contract:

```text
configSUPPORT_STATIC_ALLOCATION == 1
INCLUDE_vTaskDelete == 1
INCLUDE_vTaskSuspend == 1
INCLUDE_xTaskGetCurrentTaskHandle == 1
```

Stack telemetry is advertised independently. `SupportsStackTelemetry` is true only when `INCLUDE_uxTaskGetStackHighWaterMark == 1`.

## Composition contract

The provider offers `ESPressio::Platform::Execution::ExecutionContext` and advertises:

```text
CallerSuppliedStorage              = true
SupportsPriority                  = true
SupportsProcessorAffinity         = false
SupportsStackTelemetry            = configuration-dependent
ControlStorageBytes               = sizeof(StaticTask_t)
ControlStorageAlignment           = alignof(StaticTask_t)
StackStorageAlignment             = alignof(StackType_t)
StackAllocationGranularityBytes   = sizeof(StackType_t)
JoinWaitResolutionNanoseconds     = ceil(1 second / configTICK_RATE_HZ)
```

Application Composition selects this provider when the portable FreeRTOS task API satisfies the application's Platform execution requirements.

## Static storage and memory ownership

`Initialize()` requires caller-supplied task-control storage and caller-supplied stack storage.

The control region must be large and aligned enough for `StaticTask_t`.

The stack region must:

```text
be non-null
be aligned for StackType_t
contain at least one StackType_t element
have a byte count exactly divisible by sizeof(StackType_t)
fit configSTACK_DEPTH_TYPE after conversion from bytes to stack elements
```

The provider owns two additional `StaticSemaphore_t` objects internally for the start gate and completion notification. Those semaphores are also created through static FreeRTOS APIs.

The native task control block, native task stack, start gate, and completion signal therefore do not require FreeRTOS heap allocation through this provider.

## Initialization

`Initialize()` binds:

```text
caller storage
priority
optional name
entry function
entry parameter
```

Priority must be less than `configMAX_PRIORITIES`, and the entry function must be non-null.

The portable FreeRTOS provider does not support processor affinity. Any configuration other than `ProcessorAffinity::Any()` returns `UnsupportedAffinity`.

Important initialization outcomes include:

```text
Succeeded
AlreadyInitialized
UnsupportedAffinity
InvalidConfiguration
InvalidStorage
ProviderFailure
```

## Lifecycle

The lifecycle is:

```text
Initialize
Start
Join
Destroy
```

`Initialize()` creates the native task immediately through `xTaskCreateStatic`, but the task begins behind a private binary-semaphore start gate.

`Start()` releases that gate exactly once and permits user code to execute.

When the user entry function returns, the native task signals completion and suspends itself. The provider does not self-delete the native task.

`Join()` waits for completion and records successful ownership join. A context cannot join itself. Repeated joins after a successful join report success without waiting again.

`Destroy()` succeeds only for an initialized context that:

```text
has never been started
or
has completed and been joined
```

The provider deliberately does not forcibly delete a running or unjoined execution scope. Successful destruction calls `vTaskDelete()` and resets the provider to its uninitialized state.

## Wait semantics

`Join()` accepts `ESPressio::Platform::Synchronization::WaitTimeout`.

Finite nanosecond waits are rounded upward to FreeRTOS ticks, so conversion does not cause an earlier timeout than requested.

The wait helper carries one total finite budget across the native wait sequence and chunks very long waits below the `portMAX_DELAY` sentinel where required.

`WaitForever` maps to `portMAX_DELAY`; provider availability already requires `INCLUDE_vTaskSuspend == 1`, which is the FreeRTOS configuration required for indefinite blocking semantics.

## Stack telemetry

When enabled, `uxTaskGetStackHighWaterMark()` reports the minimum remaining stack in `StackType_t` elements. The provider converts that quantity to bytes before exposing `ExecutionStackTelemetry` and saturates at `UINT32_MAX` if necessary.

When the native telemetry API is unavailable, the provider advertises `SupportsStackTelemetry = false` and returns an unavailable telemetry reading.

## Current-context inspection and yielding

`IsCurrentContext()` compares the provider's task handle with `xTaskGetCurrentTaskHandle()`.

`Yield()` maps to `taskYIELD()` and exposes only the Platform-native yield operation. It does not define higher Threading semantics.

## Supported and unsupported behavior

Supported:

```text
caller-supplied static task storage
priority
join with ESPressio timeout policy
conditional stack telemetry
explicit native yield
```

Not provided:

```text
processor affinity
forced cancellation
automatic deletion when the entry function returns
dynamic task-stack allocation through the Platform API
Task or Thread semantic ownership
pooling
higher scheduling policy
```

Processor-affinity requirements must be satisfied by another provider, such as the ESP-IDF-specific `ExecutionContext` provider where appropriate.
