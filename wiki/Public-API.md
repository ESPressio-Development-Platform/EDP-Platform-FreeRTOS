# Public API

Concrete provider types satisfy the corresponding EDP-Platform capabilities. Provider availability is conditional on applicable FreeRTOS configuration.

CountingSemaphore and Signal expose interrupt-side operations where the FreeRTOS FromISR APIs genuinely satisfy the abstract contract. ExecutionContext exposes the Platform lifecycle Initialize, Start, Join and Destroy without forced task termination.

Exact declarations, template parameters and signatures remain authoritative in the headers exported by `ESPressio_Platform_FreeRTOS.hpp`.
