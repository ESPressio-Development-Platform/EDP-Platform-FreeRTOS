# Resources, Lifecycle and Concurrency

Synchronization primitives use static FreeRTOS allocation. ExecutionContext requires caller-supplied StaticTask_t and stack backing plus statically owned start/completion synchronization. Processor affinity is not advertised by the generic FreeRTOS provider. ISR capability is explicit per operation.
