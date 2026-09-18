# ESPressio Platform FreeRTOS

`EDP-Platform-FreeRTOS` provides concrete `EDP-Platform` implementations backed specifically by portable FreeRTOS APIs.

## Providers

Depending on the active FreeRTOS configuration, this repository can provide:

```text
ExecutionContext
Mutex
RecursiveMutex
ReadWriteMutex
CountingSemaphore
Signal
```

`SpinLock` is not provided because vanilla FreeRTOS does not define one portable cross-port spin-lock API.

`AtomicWord32` is owned by `EDP-Platform-Portable` because its current implementation uses only Standard C++ `std::atomic`.

## Documentation

- [ExecutionContext Provider](./docs/EXECUTION-CONTEXT.MD)  
  Defines static task/control storage, Composition properties, configuration gates, lifecycle, waits, stack telemetry, and unsupported execution semantics.

- [Synchronization Providers](./docs/SYNCHRONIZATION.MD)  
  Defines the FreeRTOS mutex, reader/writer lock, counting semaphore, signal, timeout conversion, interrupt operations, and static-resource contracts.

## Dependencies

```text
EDP-Platform-FreeRTOS
└── EDP-Platform [mandatory]
    └── EDP-System [transitive]
```

There are no optional ESPressio library dependencies.

See [DEPENDENCY_GRAPH.MD](./DEPENDENCY_GRAPH.MD).

## Demos

Buildable demonstrations are provided under [`./demos`](./demos).
