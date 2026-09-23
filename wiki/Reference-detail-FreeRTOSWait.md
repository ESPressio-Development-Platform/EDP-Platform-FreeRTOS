# src/detail/FreeRTOSWait.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `85baafa13f13d7d647e7a2519c17f989c72b4f15`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Platform-FreeRTOS/blob/85baafa13f13d7d647e7a2519c17f989c72b4f15/src/detail/FreeRTOSWait.hpp)

## Direct includes

- `cstdint`
- `limits`
- `FreeRTOSHeaders.hpp`
- `ESPressio_Platform.hpp`

## Documented declarations

### `NanosecondsPerSecond`

**Classification:** PRIVATE IMPLEMENTATION

Number of nanoseconds in one second.

```cpp
inline constexpr std::uint64_t NanosecondsPerSecond = 1000000000ULL;
```

### `WaitResolutionNanoseconds`

**Classification:** PRIVATE IMPLEMENTATION

FreeRTOS tick resolution rounded upward to whole nanoseconds.

```cpp
inline constexpr std::uint64_t WaitResolutionNanoseconds =
        (NanosecondsPerSecond + static_cast<std::uint64_t>(configTICK_RATE_HZ) - 1ULL) /
        static_cast<std::uint64_t>(configTICK_RATE_HZ);
```

### `ToTotalTicks`

**Classification:** PRIVATE IMPLEMENTATION

Converts a finite nanosecond duration into a rounded-up 64-bit FreeRTOS tick budget.

```cpp
inline std::uint64_t ToTotalTicks(
        ESPressio::Platform::Synchronization::WaitTimeout timeout
    ) noexcept
```

### `WaitBudget`

**Classification:** PRIVATE IMPLEMENTATION

Tracks one total FreeRTOS wait budget across one or more native semaphore acquisitions.

```cpp
class WaitBudget final
```

### `_timeout`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Original ESPressio wait request.

```cpp
ESPressio::Platform::Synchronization::WaitTimeout _timeout;
```

### `_remainingTicks`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Remaining finite wait budget expressed in native ticks.

```cpp
std::uint64_t _remainingTicks;
```

### `_lastTick`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Last observed native tick coordinate.

```cpp
TickType_t _lastTick;
```

### `ConsumeElapsed`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Removes native time elapsed since the previous budget observation.

```cpp
void ConsumeElapsed() noexcept
```

### `ConsumeWait`

**Classification:** PRIVATE IMPLEMENTATION · source access: `private`

Removes elapsed time after one finite native wait operation.

```cpp
void ConsumeWait(
                TickType_t before,
                TickType_t after,
                std::uint64_t requestedChunk,
                bool succeeded
            ) noexcept
```

### `WaitBudget`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Creates a wait budget from one ESPressio timeout.

```cpp
explicit WaitBudget(
                ESPressio::Platform::Synchronization::WaitTimeout timeout
            ) noexcept :
                _timeout(timeout),
```

### `Take`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Takes one ordinary FreeRTOS semaphore while preserving the complete wait budget.

```cpp
bool Take(
                SemaphoreHandle_t handle
            ) noexcept
```

### `TakeRecursive`

**Classification:** PRIVATE IMPLEMENTATION · source access: `public`

Takes one recursive FreeRTOS mutex while preserving the complete wait budget.

```cpp
bool TakeRecursive(
                SemaphoreHandle_t handle
            ) noexcept
```

## Preprocessor controls in this header

- `#if ( configUSE_RECURSIVE_MUTEXES == 1 )`
