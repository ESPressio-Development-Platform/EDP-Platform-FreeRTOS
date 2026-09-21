# Demos

The FreeRTOS concrete provider repository includes:

- `synchronization-basic` — mutex, recursive mutex, read/write mutex, counting semaphore and latched signal providers;
- `execution-context-basic` — caller-storage-backed static FreeRTOS execution context.

Each demo provides Arduino IDE, PlatformIO Arduino and PlatformIO ESP-IDF surfaces as required by ESPressio policy.

The selected FreeRTOS configuration must enable the static primitives used by the corresponding provider.

## C++ Language Baseline

All PlatformIO-compatible demos require C++20 and explicitly select the maintained PIOArduino ESP32 platform package.
