# Dependency Contracts

EDP-Platform-FreeRTOS depends on **EDP-Platform** and implements Platform contracts using the FreeRTOS API.

## ExecutionContext

`ExecutionContextProvider` offers the Platform `ExecutionContext` capability with:

- caller-supplied storage: true;
- priority support: true;
- processor affinity: false;
- stack telemetry: conditional on the FreeRTOS configuration;
- control storage: `sizeof(StaticTask_t)`;
- control alignment: `alignof(StaticTask_t)`;
- stack alignment: `alignof(StackType_t)`;
- stack granularity: `sizeof(StackType_t)`;
- join wait resolution: the FreeRTOS tick-derived provider value.

The provider is validated by the internal EDP-Platform `ExecutionContextProviderTraits` contract.

## Synchronization

The repository supplies concrete Mutex, RecursiveMutex, ReadWriteMutex, CountingSemaphore and Signal providers. Each is validated against the matching internal EDP-Platform provider-trait contract.

CountingSemaphore advertises interrupt release only when supported by the FreeRTOS implementation; Signal likewise advertises interrupt notification through the FromISR APIs. ReadWriteMutex is assembled from FreeRTOS primitives and preserves its higher-level contract rather than exposing native handles.

## Explicit non-contracts

This repository does not supply AtomicWord32 (owned by the Portable standard-C++ implementation) or a generic SpinLock (vanilla FreeRTOS has no single portable cross-port spin-lock API).

## Lifetime/resource boundary

ExecutionContext uses caller-provided task-control and stack backing; synchronization providers use static FreeRTOS allocation. Higher layers own provider objects.

> Dependency contract audit baseline: `c74a08b11f65f970f580d86a9262ab512e901ab3` (`main`).
