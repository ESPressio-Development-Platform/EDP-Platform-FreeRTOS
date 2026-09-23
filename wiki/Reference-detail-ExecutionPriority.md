# src/detail/ExecutionPriority.hpp

**Primary classification:** PRIVATE IMPLEMENTATION

**Source baseline:** `85baafa13f13d7d647e7a2519c17f989c72b4f15`

[Open exact source](https://github.com/ESPressio-Development-Platform/EDP-Platform-FreeRTOS/blob/85baafa13f13d7d647e7a2519c17f989c72b4f15/src/detail/ExecutionPriority.hpp)

## Direct includes

- `freertos/FreeRTOS.h`
- `ESPressio_Platform.hpp`

## Documented declarations

### `NativePriorityFor`

**Classification:** PRIVATE IMPLEMENTATION

Maps one portable ESPressio execution-priority level onto the configured native FreeRTOS priority range.

The mapping is monotonic. Critical uses the highest native priority, Low avoids the
idle priority whenever the native scheduler exposes at least one non-idle level, and
small native ranges may intentionally collapse adjacent ESPressio levels.

```cpp
constexpr UBaseType_t NativePriorityFor(
        ESPressio::Platform::Execution::ExecutionPriority priority
    ) noexcept
```

