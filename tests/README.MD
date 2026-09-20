# FreeRTOS Provider Tests

`FreeRTOSProviderTests.cpp` exercises every synchronization provider made available by the selected FreeRTOS configuration and compile-time validates the four-level execution-priority mapping against the configured native priority range.

The provider headers themselves also instantiate the corresponding `EDP-Platform` contract traits, so an incompatible provider surface fails compilation at the provider definition boundary.

Execution-context runtime behavior is demonstrated by the buildable `execution-context-basic` demo and requires a FreeRTOS target runtime.
