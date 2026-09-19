# EDP-Platform-FreeRTOS Dependency Graph

> Revalidated: 2026-09-19 — execution-priority contract alignment introduced no new ESPressio-library dependency.

## Library Dependency Tree

```text
EDP-Platform-FreeRTOS
└── EDP-Platform [mandatory]
    └── EDP-System [transitive]
```

## Mandatory Dependencies

- `EDP-Platform` — defines the Platform abstractions and provider contracts implemented by the FreeRTOS concrete providers.

## Optional Dependencies

`EDP-Platform-FreeRTOS` has no optional ESPressio library dependencies.

FreeRTOS itself is the underlying concrete runtime and is not an ESPressio-library dependency.
