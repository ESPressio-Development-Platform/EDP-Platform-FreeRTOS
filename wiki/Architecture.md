# Architecture

The repository implements ExecutionContext, Mutex, RecursiveMutex, ReadWriteMutex, CountingSemaphore and Signal using FreeRTOS primitives. It intentionally does not provide SpinLock because vanilla FreeRTOS has no single portable cross-port spinlock API; AtomicWord32 belongs to the Portable provider.

ExecutionContext uses static FreeRTOS task creation with caller-supplied TCB and stack. Start/completion coordination is also statically backed.
