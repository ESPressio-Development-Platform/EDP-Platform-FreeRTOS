# EDP-Platform-FreeRTOS Dependency Graph

## Library Dependency Tree

```text
EDP-Platform-FreeRTOS
└── EDP-Platform [mandatory]
    └── EDP-System [mandatory, transitive]
```

## Mandatory Dependencies

- `EDP-Platform` — owns the platform-neutral capability contracts implemented by this FreeRTOS backend.
- `EDP-System` — transitive through `EDP-Platform` for Composition.

## Optional Dependencies

There are currently no optional ESPressio library dependencies.

FreeRTOS/toolchain facilities and the C++ standard atomic implementation are target facilities, not ESPressio library dependencies.
