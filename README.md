# QXK24 – Universal Kernel

> **ALAMTOLOGI – Quranic Science**
> Founder: Masa Bayu · masaqiubbx@gmail.com
> Version: 1.0.0 · Released: 2026-05-25

[![Version](https://img.shields.io/badge/version-1.0.0-blue)](https://github.com/qxengine/qxk24/releases/tag/v1.0.0)
[![Tests](https://img.shields.io/badge/tests-516%2F516-brightgreen)](https://github.com/qxengine/qxk24)
[![Certification](https://img.shields.io/badge/certification-SOVEREIGN-gold)](https://github.com/qxengine/qxk24)
[![License](https://img.shields.io/badge/license-Proprietary-red)](https://github.com/qxengine/qxk24)

---

## What is QXK24?

QXK24 is the **world's first constitutional digital kernel**
built on the science of **ALAMTOLOGI — Quranic Science**.

It is not an engine. It is not a framework. It is a **kernel** —
a constitutional master that governs all digital resources
under eight immutable laws derived from Quranic science.

Every resource governed by QXK24 is measured, certified,
and validated. Nothing is ungoverned.

---

## K24 Hierarchy

```
QXK24    (K24md — Modul  — scale 46,656)   ← MASTER
  └── QXEngine (K24ob — Objek  — scale 1,296)   Memory Governor
        └── QXMemloc (K24at — Atom  — scale 36)   Memory Locator
```

| Component | Role | K24 Level | Repository |
|---|---|---|---|
| QXK24 | Universal Kernel — MASTER | K24md — Modul | [qxk24](https://github.com/qxengine/qxk24) |
| QXEngine | Memory Governor | K24ob — Objek | [qxengine-core](https://github.com/qxengine/qxengine-core) |
| QXMemloc | Memory Locator | K24at — Atom | [qxengine-core](https://github.com/qxengine/qxengine-core) |

---

## Eight Constitutional Laws

All resources, flows, and cycles are governed by
eight immutable constitutional laws:

| Law | Name | Weight |
|---|---|---|
| Z.1 | Existence | 0.20 |
| Z.2 | Pair | 0.20 |
| Z.3 | Direction | 0.15 |
| Z.4 | Measurement | 0.15 |
| X.1 | Flow | 0.12 |
| X.2 | Cross | 0.10 |
| X.3 | Cycle | 0.05 |
| X.4 | Constitution | 0.03 |

Universal measurement formula: **X = m/t**

---

## Six Constitutional Directions

| Direction | Code | Role | Pair |
|---|---|---|---|
| Top | As | Governance | ↕ Bh |
| Bottom | Bh | Hardware | ↕ As |
| Front | Dn | Execution | ↕ Bg |
| Back | Bg | Storage | ↕ Dn |
| Right | Kn | Output | ↕ Kr |
| Left | Kr | Input | ↕ Kn |

---

## Seven K24 Levels

| Level | Name | Scale |
|---|---|---|
| 1 | Zarah | 1 |
| 2 | Jisim | 6 |
| 3 | Atom | 36 |
| 4 | Sistem | 216 |
| 5 | Objek | 1,296 |
| 6 | Aktif | 7,776 |
| 7 | Modul | 46,656 |

---

## Certification Tiers

| Score | Tier |
|---|---|
| ≥ 95.0 | SOVEREIGN |
| ≥ 80.0 | PROFESSIONAL |
| ≥ 60.0 | STANDARD |
| < 60.0 | UNCERTIFIED |

---

## Quick Start

### C

```c
/* 1. Create and start kernel */
QXK24Config cfg = qxk24_default_config();
qxk24_kernel_t kernel = qxk24_create(&cfg);
qxk24_start(kernel);

/* 2. Register all six governors */
qxk24_register_resource(kernel, QXK24_DIR_AS,
    qxk24_resource_memory_create(256 * 1024 * 1024));
qxk24_register_resource(kernel, QXK24_DIR_DN,
    qxk24_resource_cpu_create(100));
qxk24_register_resource(kernel, QXK24_DIR_KN,
    qxk24_resource_network_create(100 * 1024 * 1024));
qxk24_register_resource(kernel, QXK24_DIR_BG,
    qxk24_resource_storage_create(512 * 1024 * 1024));
qxk24_register_resource(kernel, QXK24_DIR_BH,
    qxk24_resource_io_create(64));
qxk24_register_resource(kernel, QXK24_DIR_KR,
    qxk24_resource_energy_create(100));

/* 3. Cycle and read health */
qxk24_cycle(kernel);
float health = qxk24_health_score(kernel);

/* 4. Generate constitutional report */
QXK24Report report;
qxk24_report_generate(kernel, &report);

/* 5. Teardown */
qxk24_stop(kernel);
qxk24_destroy(kernel);
```

### Swift (iOS)

```swift
try QXK24Engine.shared.start()
try QXK24Engine.shared.registerAllGovernors()
try QXK24Engine.shared.cycle()

let health = QXK24Engine.shared.healthScore
let report = try QXK24Engine.shared.generateReport()

QXK24Engine.shared.onHealthChanged { snapshot in
    print("Health: \(snapshot.score) — \(snapshot.tier.label)")
}
```

### Kotlin (Android)

```kotlin
QXK24Engine.start()
QXK24Engine.registerAllGovernors()
QXK24Engine.cycle()

val health = QXK24Engine.healthScore
val report = QXK24Engine.generateReport()

QXK24Engine.onHealthChanged = { snapshot ->
    println("Health: ${snapshot.score} — ${snapshot.tier.label}")
}
```

---

## Build

```bash
git clone https://github.com/qxengine/qxk24.git
cd qxk24
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && ctest --output-on-failure
```

**Expected:**

```
516/516 tests passed
```

---

## Release History

| Tag | Tests | Certification |
|---|---|---|
| v1.0.0-alpha.1 | 84/84 | — |
| v1.0.0-alpha.2 | 182/182 | — |
| v1.0.0-alpha.3 | 302/302 | STANDARD ≥ 60.0 |
| v1.0.0-alpha.4 | 362/362 | PROFESSIONAL ≥ 80.0 |
| v1.0.0-beta.1 | 434/434 | PROFESSIONAL ≥ 80.0 |
| **v1.0.0** | **516/516** | **SOVEREIGN ≥ 95.0** |

---

## Repository Map

| Repo | Role |
|---|---|
| [qxk24](https://github.com/qxengine/qxk24) | Universal Kernel — MASTER |
| [qxengine-core](https://github.com/qxengine/qxengine-core) | Memory Governor — K24ob |
| [qxengine-ios](https://github.com/qxengine/qxengine-ios) | iOS Bridge |
| [qxengine-android](https://github.com/qxengine/qxengine-android) | Android Bridge |

---

## About

**ALAMTOLOGI – Quranic Science**
**Founder:** Masa Bayu
**Contact:** masaqiubbx@gmail.com
**Organization:** [QXK24 – Universal Kernel](https://github.com/qxengine)
**Released:** 2026-05-25

> *QXK24 is my contribution to showing that technology
> and faith are not opposites — they are constitutionally
> aligned.*
>
> — Masa Bayu
