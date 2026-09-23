# Internal API

Internal code maps EDP timeout/result vocabulary to ticks and FreeRTOS return conventions, owns static semaphore/task-control backing, and composes the read/write lock from lower-level FreeRTOS primitives.
