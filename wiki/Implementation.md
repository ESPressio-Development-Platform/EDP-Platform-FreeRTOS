# Private Implementation

ReadWriteMutex uses a reader-count guard mutex, writer turnstile and resource binary semaphore. The turnstile prevents indefinite reader bypass once a writer is waiting.

Finite waits are rounded conservatively to ticks while preserving a total timeout budget. Destroy does not forcibly terminate a still-running context.
