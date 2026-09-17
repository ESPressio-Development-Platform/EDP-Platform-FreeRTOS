# Demos

- `atomic-word-basic` — the FreeRTOS `AtomicWord32` concrete provider.

The provider currently relies only on the standard lock-free 32-bit atomic contract and therefore remains buildable in each policy-required target project without changing its FreeRTOS domain ownership.
