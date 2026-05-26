# QXK24 Universal Kernel
## QXEngine Integration Guide

**Version:** 1.0.0-beta.1  
**Date:** 2026-05-25  
**Founder:** Masa Bayu  
**Repository:** https://github.com/qxk24/qxk24

## ALAMTOLOGI

QXEngine is the K24ob memory governor inside QXK24. It operates at the
Objek level of the K24 hierarchy and feeds constitutional health into the
QXK24 global cycle at direction As, the governance direction.

```text
QXK24 (K24md - Modul)        Universal Kernel
  QXEngine (K24ob - Objek)   Memory Governor
    QXMemloc (K24at - Atom)  Memory Locator
```

## Health Formula

The integration bridge computes combined health with the kernel as the
majority signal and QXEngine as the memory-governor contribution:

```text
combined = kernel_health * 0.60 + qxengine_health * 0.40
```

QXK24 beta.1 targets PROFESSIONAL certification, so the combined score must
remain at or above 80.0.

## Quick Start

```c
QXK24Config cfg = qxk24_default_config();
qxk24_kernel_t kernel = NULL;
qxk24_create(&cfg, &kernel);
qxk24_start(kernel);

QXK24QXEngineConfig gcfg = qxk24_qxengine_default_config();
qxk24_qxengine_t gov = qxk24_qxengine_create(&gcfg);
qxk24_qxengine_start(gov);

qxk24_bridge_t bridge = qxk24_bridge_create(kernel, gov);
qxk24_bridge_cycle(bridge);

QXK24BridgeSnapshot snap;
qxk24_bridge_snapshot(bridge, &snap);

qxk24_bridge_destroy(bridge);
qxk24_qxengine_destroy(gov);
qxk24_stop(kernel);
qxk24_destroy(kernel);
```

## Bridge States

`QXK24_BRIDGE_DETACHED` means QXEngine has not yet been registered into the
kernel. `QXK24_BRIDGE_ATTACHED` means the K24ob memory resource is registered
at direction As. `QXK24_BRIDGE_ACTIVE` means synchronized cycles are running
and the combined score meets the PROFESSIONAL target. `QXK24_BRIDGE_FAULTED`
means a kernel, resource, or governor operation failed.

## Cycle Order

Each synchronized cycle follows one direction:

```text
QXEngine cycle -> QXK24 resource update -> QXK24 kernel cycle -> health feed
```

The bridge stores the latest QXEngine health, kernel health, combined health,
and synchronized cycle count in `QXK24BridgeSnapshot`.

## Constitutional Placement

QXEngine is registered as a memory resource at K24ob and direction As. This
keeps QXEngine inside the governance path while QXK24 remains the K24md
universal kernel.

## Certification Path

```text
SOVEREIGN     >= 95.0
PROFESSIONAL  >= 80.0
STANDARD      >= 60.0
UNCERTIFIED   <  60.0
```

Phase 5 proves QXEngine as the memory governor and confirms the PROFESSIONAL
target through native integration tests.
