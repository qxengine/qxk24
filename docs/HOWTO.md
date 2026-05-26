# QXK24 Universal Kernel
## Developer HOWTO: Technical Reference for Building Apps

**ALAMTOLOGI - Quranic Science**  
**Founder:** Masa Bayu  
**Repository:** https://github.com/qxk24/qxk24  
**Current Version:** v1.3.0  
**Audience:** Developers, app architects, technical founders, mobile engineers, backend engineers, AI engineers, IoT engineers, and platform teams.

---

## 1. Purpose of This HOWTO

This HOWTO is the practical technical reference for developers who want to build apps with QXK24. It is section-by-section implementation-focused, and designed to answer the question:

```text
How do I use QXK24 to build a real app that is measured,
governed, reportable, and ready for release?
```

This document is not a philosophy paper. It is a developer guide. It explains
what to build, what to measure, where to place QXK24, which APIs to call, which
reports to generate, and how to prove that an app is ready.

Use this document together with:

- `docs/WHITEPAPER.md` for the general technical vision.
- `docs/DEVELOPER_SOP.md` for the operating procedure.
- `docs/DEVELOPER_WHITEPAPER.md` for detailed developer findings.
- `docs/BUSINESS_WHITEPAPER.md` for business positioning.
- `docs/FOUNDER_STATEMENT.md` for the founder declaration.

---

## 2. What QXK24 Adds to an App

Most frameworks help developers build features. QXK24 helps developers govern
the digital body behind those features.

QXK24 adds:

- Kernel lifecycle: create, start, cycle, stop, destroy.
- Resource registration for memory, CPU, network, storage, I/O, and energy.
- Constitutional measurement using `X = m / t`.
- Direct and cross-flow tracking through OAT and OASS.
- Constitutional health scoring.
- Certification tiers: NONE, STANDARD, PROFESSIONAL, SOVEREIGN.
- Reports for release and runtime decisions.
- Faktor Masa for time boundaries.
- Faktor Tenaga for energy visibility.
- Caliph judgment for continue, correct, or stop decisions.
- QXEngine and QXMemloc integration for memory governance.
- QIUBBX validation for SOVEREIGN proof.
- Platform bridge patterns for iOS and Android.

The result is an app that can answer:

```text
What exists?
Where does it move?
How much does it consume?
How is it measured?
What is its health?
Should it continue, correct, or stop?
```

---

## 3. When to Use QXK24

Use QXK24 when the app has one or more of these conditions:

- It uses significant memory, network, storage, CPU, I/O, or energy.
- It has background work, retry loops, sync, queues, or scheduled jobs.
- It uses AI, model inference, prompts, tools, or vector memory.
- It handles money, identity, private data, health data, or audit records.
- It needs release proof beyond ordinary unit tests.
- It needs cross-platform governance across C, Swift, Kotlin, or backend code.
- It must explain system health to developers, operators, or business owners.

Do not wait until production incidents appear. QXK24 is easiest to adopt when
resource ownership and lifecycle boundaries are defined early.

---

## 4. Core Mental Model

Think of an app as a governed system:

```text
App Feature Layer
  screens, APIs, jobs, workflows, AI calls, device tasks

QXK24 Governance Layer
  kernel, resources, flows, cycles, health, reports, judgment

Runtime Platform Layer
  OS, device, cloud, database, network, storage, model runtime
```

Feature code should remain simple. Governance should live in a small adapter or
service layer that calls QXK24.

Recommended app-level wrapper:

```text
AppGovernance
  start()
  registerResources()
  beginWork(name)
  updateResource(resource, used, elapsed)
  endWork(name)
  cycle()
  report()
  decision()
  stop()
```

This keeps QXK24 practical. Developers should not scatter raw kernel calls
through every feature file unless the app is very small.

---

## 5. Public API Map

### 5.1 Core Header

Use:

```c
#include "qxk24/qxk24.h"
```

Core lifecycle APIs:

| API | Purpose |
|---|---|
| `qxk24_default_config()` | Build a default kernel config. |
| `qxk24_create()` | Create a kernel handle. |
| `qxk24_start()` | Start the kernel. |
| `qxk24_cycle()` | Run one constitutional cycle and optionally output a report. |
| `qxk24_stop()` | Stop the kernel. |
| `qxk24_destroy()` | Destroy the kernel. |

Core query APIs:

| API | Purpose |
|---|---|
| `qxk24_health_score()` | Read current health score. |
| `qxk24_cert_tier()` | Read current certification tier. |
| `qxk24_cycle_index()` | Read current cycle index. |
| `qxk24_active_resources()` | Count active resources. |
| `qxk24_active_flows()` | Count active flows. |
| `qxk24_get_law_scores()` | Read the eight-law score vector. |

Resource APIs:

| API | Purpose |
|---|---|
| `qxk24_resource_register()` | Register one governed resource. |
| `qxk24_resource_update()` | Update resource usage and measured `X`. |
| `qxk24_resource_release()` | Release a registered resource. |

Flow APIs:

| API | Purpose |
|---|---|
| `qxk24_flow_begin()` | Begin an app-level flow. |
| `qxk24_flow_complete()` | Complete an app-level flow. |
| `qxk24_flow_cancel()` | Cancel an app-level flow. |

Report APIs:

```c
#include "qxk24/kernel/qxk24_report.h"
```

| API | Purpose |
|---|---|
| `qxk24_report_generate()` | Generate a constitutional report. |
| `qxk24_report_is_sound()` | Check whether the report is sound. |
| `qxk24_report_print()` | Print a report for CLI/debug use. |

---

## 6. Technical Reference Data

Developers should treat these values as the current QXK24 v1.3.0 technical
baseline.

### 6.1 Version and Build Data

| Item | Value |
|---|---|
| Current core version | `1.3.0` |
| Version tag string | `release` |
| Core language | C11 |
| Test language | C++17 |
| Core target | `qxk24_core` |
| Test target | `qxk24_tests` |
| Threading dependency | `pthread` |
| Native test suite target | GoogleTest |
| Current release standing | SOVEREIGN maintained |

### 6.2 Kernel Limits

| Constant | Value | Meaning |
|---|---|---|
| `QXK24_LEVEL_COUNT` | `7` | Seven K24 levels. |
| `QXK24_DIRECTION_COUNT` | `6` | Six constitutional directions. |
| `QXK24_LAW_COUNT` | `8` | Eight constitutional laws. |
| `QXK24_CYCLE_INTERVAL_MS` | `5000` | Default cycle interval. |
| `QXK24_MAX_RESOURCES` | `256` | Maximum active resource records. |
| `QXK24_MAX_ACTIVE_FLOWS` | `512` | Maximum active kernel flow records. |
| `QXK24_MAX_LABEL_LEN` | `64` | Maximum label length. |
| `QXK24_MAX_LOG_LEN` | `512` | Maximum report log length. |

### 6.3 Certification Thresholds

| Tier | Constant | Score |
|---|---|---|
| SOVEREIGN | `QXK24_SCORE_SOVEREIGN` | `95.0` |
| PROFESSIONAL | `QXK24_SCORE_PROFESSIONAL` | `80.0` |
| STANDARD | `QXK24_SCORE_STANDARD` | `60.0` |
| NONE | `QXK24_CERT_NONE` | `< 60.0` |

### 6.4 Eight-Law Weights

| Law | Constant | Weight |
|---|---|---|
| Z.1 | `QXK24_WEIGHT_Z1` | `0.20` |
| Z.2 | `QXK24_WEIGHT_Z2` | `0.20` |
| Z.3 | `QXK24_WEIGHT_Z3` | `0.15` |
| Z.4 | `QXK24_WEIGHT_Z4` | `0.15` |
| X.1 | `QXK24_WEIGHT_X1` | `0.12` |
| X.2 | `QXK24_WEIGHT_X2` | `0.10` |
| X.3 | `QXK24_WEIGHT_X3` | `0.05` |
| X.4 | `QXK24_WEIGHT_X4` | `0.03` |

### 6.5 K24 Scale Data

| Level | Constant | Scale |
|---|---|---|
| Zarah | `QXK24_SCALE_ZARAH` | `1` |
| Jisim | `QXK24_SCALE_JISIM` | `6` |
| Atom | `QXK24_SCALE_ATOM` | `36` |
| Sistem | `QXK24_SCALE_SISTEM` | `216` |
| Objek | `QXK24_SCALE_OBJEK` | `1,296` |
| Aktif | `QXK24_SCALE_AKTIF` | `7,776` |
| Modul | `QXK24_SCALE_MODUL` | `46,656` |

### 6.6 Error Codes

| Code | Meaning |
|---|---|
| `QXK24_OK` | Success. |
| `QXK24_ERR_INVALID_ARGUMENT` | Invalid input, null parameter, or bad enum. |
| `QXK24_ERR_OUT_OF_MEMORY` | Allocation failure. |
| `QXK24_ERR_NOT_INITIALIZED` | Kernel or component is not initialized. |
| `QXK24_ERR_ALREADY_INITIALIZED` | Component is already initialized. |
| `QXK24_ERR_BUDGET_EXCEEDED` | Resource budget exceeded. |
| `QXK24_ERR_LEVEL_MISMATCH` | Invalid level relationship. |
| `QXK24_ERR_DIRECTION_FAULT` | Invalid direction or pair. |
| `QXK24_ERR_FLOW_VIOLATION` | Flow law violation. |
| `QXK24_ERR_LAW_VIOLATION` | Constitutional law violation. |
| `QXK24_ERR_NOT_FOUND` | Requested ID or record not found. |
| `QXK24_ERR_CAPACITY_FULL` | Capacity limit reached. |

---

## 7. Build QXK24

From the `qxk24` repository:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && ctest --output-on-failure
```

The core target is:

```text
qxk24_core
```

The test executable is:

```text
qxk24_tests
```

The build uses:

- C11 for core C code.
- C++17 for tests.
- `pthread` for synchronization.
- GoogleTest for proof tests.

---

## 8. Add QXK24 to a C or C++ App

### 8.1 CMake Integration Pattern

If QXK24 is included as a subdirectory:

```cmake
add_subdirectory(path/to/qxk24)

add_executable(my_app
    src/main.c
)

target_link_libraries(my_app PRIVATE qxk24_core)
target_include_directories(my_app PRIVATE path/to/qxk24/include)
```

If QXK24 is built separately, link the static library and include headers:

```cmake
target_include_directories(my_app PRIVATE /path/to/qxk24/include)
target_link_libraries(my_app PRIVATE /path/to/qxk24/build/libqxk24_core.a pthread)
```

### 8.2 Minimum C Integration

```c
#include "qxk24/qxk24.h"
#include "qxk24/kernel/qxk24_report.h"

int main(void) {
    QXK24Config cfg = qxk24_default_config();
    QXK24KernelHandle kernel = NULL;

    if (qxk24_create(&cfg, &kernel) != QXK24_OK) {
        return 1;
    }

    if (qxk24_start(kernel) != QXK24_OK) {
        qxk24_destroy(kernel);
        return 1;
    }

    QXK24Report report;
    if (qxk24_cycle(kernel, &report) == QXK24_OK) {
        qxk24_report_print(&report);
    }

    qxk24_stop(kernel);
    qxk24_destroy(kernel);
    return 0;
}
```

This confirms the kernel can be created, started, cycled, reported, and
destroyed.

---

## 9. Design App Governance Before Coding Features

Before adding feature code, create these documents or internal definitions:

| Artifact | Purpose |
|---|---|
| App category | Defines the app type and primary risks. |
| Direction map | Maps modules to As, Bh, Dn, Bg, Kn, Kr. |
| Resource budget map | Defines budgets for memory, CPU, network, storage, I/O, energy. |
| K24 level map | Defines component level and responsibility. |
| Cycle schedule | Defines when QXK24 cycles run. |
| Health policy | Defines target tier and correction rules. |
| Release proof | Defines tests and reports required for release. |

This can be written in docs or encoded in an `AppGovernance` module.

---

## 10. Map the Six Directions

Every significant module should belong to one constitutional direction.

| Direction | Code | Use in Apps |
|---|---|---|
| Atas | `QXK24_DIR_AS` | Governance, orchestration, policy, scheduler. |
| Bawah | `QXK24_DIR_BH` | Hardware, device, OS, platform limits. |
| Depan | `QXK24_DIR_DN` | Active execution, user action, workers. |
| Belakang | `QXK24_DIR_BG` | Storage, history, cache, persistence. |
| Kanan | `QXK24_DIR_KN` | Output, egress, responses, notifications. |
| Kiri | `QXK24_DIR_KR` | Input, ingress, requests, prompts, sensor reads. |

Use:

```c
QXK24Direction pair = qxk24_direction_pair(QXK24_DIR_KR);
```

Example mappings:

| App Area | Direction |
|---|---|
| API request input | `QXK24_DIR_KR` |
| HTTP response | `QXK24_DIR_KN` |
| Database | `QXK24_DIR_BG` |
| Job execution | `QXK24_DIR_DN` |
| Device runtime | `QXK24_DIR_BH` |
| Policy/orchestration | `QXK24_DIR_AS` |

---

## 11. Choose K24 Levels

Use K24 levels to express scale and responsibility.

| Level | API | Use |
|---|---|---|
| Zarah | `QXK24_LEVEL_ZARAH` | Smallest unit, primitive measurement. |
| Jisim | `QXK24_LEVEL_JISIM` | Small helper or local unit. |
| Atom | `QXK24_LEVEL_ATOM` | Atomic service, locator, leaf runtime. |
| Sistem | `QXK24_LEVEL_SISTEM` | Internal subsystem. |
| Objek | `QXK24_LEVEL_OBJEK` | Governor or major object. |
| Aktif | `QXK24_LEVEL_AKTIF` | Active runtime service. |
| Modul | `QXK24_LEVEL_MODUL` | Application master or universal kernel. |

Use helpers:

```c
uint64_t scale = qxk24_level_scale(QXK24_LEVEL_MODUL);
uint64_t budget = qxk24_level_budget(QXK24_LEVEL_SISTEM);
```

Rule:

```text
Do not make every component QXK24_LEVEL_MODUL.
Use lower levels for internal components and reserve MODUL for app-level masters.
```

---

## 12. Register Resources

Resource registration is the main entry point for practical governance.

```c
uint32_t memory_id = 0;
qxk24_err_t err = qxk24_resource_register(
    kernel,
    QXK24_RESOURCE_MEMORY,
    QXK24_DIR_AS,
    QXK24_LEVEL_MODUL,
    512ULL * 1024ULL * 1024ULL,
    &memory_id
);
```

Arguments:

| Argument | Meaning |
|---|---|
| `kernel` | Running kernel handle. |
| `QXK24_RESOURCE_MEMORY` | Resource type. |
| `QXK24_DIR_AS` | Constitutional direction. |
| `QXK24_LEVEL_MODUL` | K24 level. |
| `budget_soft` | Soft budget. |
| `out_resource_id` | ID used for later updates. |

Recommended app minimum:

```c
uint32_t memory_id = 0;
uint32_t cpu_id = 0;
uint32_t network_id = 0;
uint32_t storage_id = 0;
uint32_t io_id = 0;
uint32_t energy_id = 0;

qxk24_resource_register(kernel, QXK24_RESOURCE_MEMORY,
    QXK24_DIR_AS, QXK24_LEVEL_MODUL, memory_budget, &memory_id);
qxk24_resource_register(kernel, QXK24_RESOURCE_CPU,
    QXK24_DIR_DN, QXK24_LEVEL_SISTEM, cpu_budget, &cpu_id);
qxk24_resource_register(kernel, QXK24_RESOURCE_NETWORK,
    QXK24_DIR_KN, QXK24_LEVEL_SISTEM, network_budget, &network_id);
qxk24_resource_register(kernel, QXK24_RESOURCE_STORAGE,
    QXK24_DIR_BG, QXK24_LEVEL_SISTEM, storage_budget, &storage_id);
qxk24_resource_register(kernel, QXK24_RESOURCE_IO,
    QXK24_DIR_BH, QXK24_LEVEL_ATOM, io_budget, &io_id);
qxk24_resource_register(kernel, QXK24_RESOURCE_ENERGY,
    QXK24_DIR_KR, QXK24_LEVEL_SISTEM, energy_budget, &energy_id);
```

Budget units are application-defined but should be consistent:

| Resource | Suggested Unit |
|---|---|
| Memory | Bytes |
| CPU | Milliseconds, operation cost, or percentage budget encoded as units |
| Network | Bytes |
| Storage | Bytes |
| I/O | Operation count |
| Energy | Millijoules, battery percentage units, or app-defined energy units |

---

## 13. Measure X = m / t

QXK24 expects a measured `X` when updating resources.

```text
X = m / t
```

Where:

- `m` is mass: bytes, operations, energy, CPU cost, request count, etc.
- `t` is time: milliseconds or another consistent time unit.

For resource governors, QXK24 provides:

```c
#include "qxk24/resource/qxk24_resource_base.h"

double x = qxk24_governor_measure_x(bytes, elapsed_ms);
```

If you do not use the governor helper, compute consistently:

```c
double measure_x(uint64_t mass, uint64_t elapsed_ms) {
    if (elapsed_ms == 0) return 0.0;
    return (double)mass / (double)elapsed_ms;
}
```

Update the kernel:

```c
qxk24_resource_update(kernel, memory_id, used_bytes, x_measured);
```

Update after:

- Allocating or freeing memory.
- Sending or receiving data.
- Running model inference.
- Completing a database query.
- Writing files.
- Running background jobs.
- Completing sensor batches.
- Finishing a UI-heavy screen.

---

## 14. Run Cycles

A cycle is a constitutional checkpoint.

```c
QXK24Report report;
qxk24_err_t err = qxk24_cycle(kernel, &report);
```

Run cycles:

- At app startup.
- After registering resources.
- After major user flows.
- Before payment commit.
- Before and after AI inference.
- After sync.
- On foreground/background transitions.
- Before release validation.
- At controlled intervals in backend workers.

Do not cycle so often that governance becomes noise. Use meaningful lifecycle
events and measured intervals.

---

## 15. Generate and Read Reports

Use `QXK24Report` as the primary technical data structure.

Important fields:

| Field | Meaning |
|---|---|
| `cycle_index` | Cycle number. |
| `timestamp_ms` | Report time. |
| `law_scores` | Eight-law score components. |
| `health_score` | Overall health score. |
| `cert_tier` | Certification tier. |
| `active_resources` | Active resource count. |
| `active_flows` | Active flow count. |
| `total_bytes_governed` | Governed byte total. |
| `global_utilisation` | Global utilization ratio. |
| `log` | Report log string. |

Example:

```c
QXK24Report report;
if (qxk24_report_generate(kernel, &report) == QXK24_OK) {
    if (!qxk24_report_is_sound(&report)) {
        /* ISLAH or WAQF handling begins here. */
    }
}
```

Suggested app policy:

| Tier | Score | App Action |
|---|---|---|
| `QXK24_CERT_SOVEREIGN` | `>= 95` | Release or continue. |
| `QXK24_CERT_PROFESSIONAL` | `>= 80` | Production acceptable. |
| `QXK24_CERT_STANDARD` | `>= 60` | Basic validation only. |
| `QXK24_CERT_NONE` | `< 60` | Do not release; correct or stop. |

---

## 16. Implement Continue, Correct, Stop

Reports must produce decisions.

```c
typedef enum AppDecision_e {
    APP_DECISION_CONTINUE = 0,
    APP_DECISION_CORRECT = 1,
    APP_DECISION_STOP = 2
} AppDecision;

AppDecision decide_from_report(const QXK24Report *report) {
    if (!report) return APP_DECISION_STOP;
    if (report->health_score >= QXK24_SCORE_PROFESSIONAL &&
        report->cert_tier >= QXK24_CERT_PROFESSIONAL) {
        return APP_DECISION_CONTINUE;
    }
    if (report->health_score >= QXK24_SCORE_STANDARD) {
        return APP_DECISION_CORRECT;
    }
    return APP_DECISION_STOP;
}
```

Correction actions:

- Reduce concurrency.
- Cancel noncritical work.
- Clear cache.
- Delay sync.
- Stop retries.
- Lower model cost.
- Pause background work.
- Ask for human intervention.

Stop actions:

- Fail closed.
- Prevent payment commit.
- Stop unsafe AI tool call.
- Pause data export.
- Block write to critical storage.
- Preserve report for audit.

---

## 17. Track Direct Flows with OAT

Use OAT for direct resource movement.

Examples:

- Memory allocation and release.
- One file write.
- One API response.
- One local computation.
- One sensor read batch.

OAT API:

```c
#include "qxk24/flow/qxk24_oat.h"

QXK24OATContext *oat = NULL;
qxk24_oat_context_create(&oat);

uint64_t flow_id = 0;
qxk24_oat_begin(
    oat,
    QXK24_FLOW_OAT_SEHALA,
    QXK24_FLOW_IKJ,
    QXK24_LEVEL_SISTEM,
    QXK24_DIR_DN,
    memory_id,
    bytes,
    &flow_id
);

QXK24OATMeasure measure;
qxk24_oat_complete(oat, flow_id, &measure);

qxk24_oat_context_destroy(oat);
```

Use OAT when one resource or one direction owns most of the work.

---

## 18. Track Cross Flows with OASS

Use OASS for cross-direction flows.

Examples:

- Payment: input, network, storage, output.
- AI tool call: prompt, model, tool, memory, response.
- Sync: local storage, network, remote state.
- Healthcare data access: identity, storage, report, audit.

OASS API:

```c
#include "qxk24/flow/qxk24_oass.h"

QXK24OASSContext *oass = NULL;
qxk24_oass_context_create(&oass);

uint64_t flow_id = 0;
qxk24_oass_begin(
    oass,
    QXK24_FLOW_OASS_SEHALA,
    QXK24_FLOW_IKJ,
    QXK24_DIR_KR,
    QXK24_DIR_BG,
    QXK24_LEVEL_SISTEM,
    QXK24_LEVEL_SISTEM,
    input_resource_id,
    storage_resource_id,
    bytes,
    &flow_id
);

QXK24OASSMeasure measure;
qxk24_oass_complete(oass, flow_id, &measure);

qxk24_oass_context_destroy(oass);
```

Use OASS when a flow crosses direction, level, or resource boundary.

---

## 19. Use Faktor Masa

Faktor Masa gives time structure to the app.

```c
#include "qxk24/fm/qxk24_fm.h"

qxk24_fm_t fm = qxk24_fm_create(kernel);
qxk24_fm_start(fm);

qxk24_fm_cycle(fm);

QXK24FmSnapshot snap;
qxk24_fm_snapshot(fm, &snap);

qxk24_fm_stop(fm);
qxk24_fm_destroy(fm);
```

Use Fm for:

- Background task windows.
- Retry boundaries.
- Session time.
- Sync windows.
- Inference time budgets.
- Device work intervals.

Key snapshot fields:

| Field | Meaning |
|---|---|
| `fm_value` | Constitutional Fm value. |
| `mp_ratio` | MP ratio. |
| `mdk_ratio` | Controlled time ratio. |
| `energy_factor` | Current `X = m/t`. |
| `flow_law` | Active time-flow law. |
| `constitutionally_timed` | Whether Fm is within bounds. |

---

## 20. Use Faktor Tenaga

Faktor Tenaga gives energy structure to the app.

```c
#include "qxk24/ft/qxk24_ft.h"

qxk24_ft_t ft = NULL;
qxk24_ft_create(kernel, &ft);
qxk24_ft_start(ft);

qxk24_ft_cycle(ft);

QXK24FtSnapshot snap;
qxk24_ft_snapshot(ft, &snap);

qxk24_ft_stop(ft);
qxk24_ft_destroy(ft);
```

Use Ft for:

- Battery-sensitive mobile work.
- AI inference cost.
- Server CPU load.
- IoT device energy.
- Repeated background work.
- Duplicate network activity.

Key snapshot fields:

| Field | Meaning |
|---|---|
| `hidup_aktif` | Active energy side. |
| `hidup_pasif` | Passive energy side. |
| `ft_total` | Total energy structure. |
| `fm_value` | Time link. |
| `x_measurement` | Current measurement. |
| `phase` | Energy phase. |
| `world` | Dtk or Dk world indicator. |

---

## 21. Use Caliph Judgment

Use Caliph when the app needs a dedicated constitutional controller above the
kernel.

```c
#include "qxk24/caliph/qxk24_caliph.h"

qxk24_caliph_t caliph = NULL;
qxk24_caliph_declare(kernel, &caliph);
qxk24_caliph_activate(caliph);

QXK24CommandResult command_result;
qxk24_caliph_command_startup(caliph, &command_result);

QXK24CaliphReport report;
qxk24_caliph_receive_report(caliph, &report);

QXK24CaliphJudgment judgment = qxk24_caliph_judge(caliph, &report);
const char *name = qxk24_caliph_judgment_name(judgment);

qxk24_caliph_end(caliph);
qxk24_caliph_destroy(caliph);
```

Judgment table:

| Judgment | Meaning | App Action |
|---|---|---|
| `QXK24_JUDGMENT_MAKMUR` | Healthy and sound | Continue. |
| `QXK24_JUDGMENT_ISLAH` | Correction needed | Repair, reduce, rebalance. |
| `QXK24_JUDGMENT_WAQF` | Unsafe or failed | Stop and protect. |

Startup command order:

```text
START
REGISTER_GOVERNORS
FM_CYCLE
CYCLE
HEALTH_CHECK
```

Use Caliph for:

- Apps with critical workflows.
- Apps requiring explicit decision authority.
- Finance, healthcare, AI tool calls, IoT control, backend release gates.

---

## 22. Integrate QXEngine and QXMemloc

QXEngine is the K24ob memory governor. QXMemloc is the K24at memory locator.

Bridge APIs:

```c
#include "qxk24/bridge/qxk24_qxengine_bridge.h"
#include "qxk24/memloc/qxk24_memloc_bridge.h"
```

Use the bridge when the app needs full-stack memory governance:

```text
QXK24    K24md  Universal Kernel
QXEngine K24ob  Memory Governor
QXMemloc K24at  Memory Locator
```

Bridge flow:

```text
create kernel
create QXEngine memory governor
create bridge
attach bridge
cycle bridge
snapshot bridge
detach and destroy
```

QXMemloc flow:

```text
create memloc config
create memloc bridge
locate memory record
release memory record
read memloc health
destroy memloc
```

Use QXMemloc for apps where memory location, ownership, and release proof are
important.

---

## 23. Run QIUBBX Validation

QIUBBX validates the constitutional proof surface.

```c
#include "qxk24/validation/qxk24_qiubbx.h"
```

Validation API:

```c
qxk24_qiubbx_t validator = qxk24_qiubbx_create(kernel, bridge);

QXK24QIUBBXReport report;
qxk24_qiubbx_run(validator, &report);

float sovereign = qxk24_qiubbx_sovereign_score(&report);
bool ok = report.sovereign_achieved;

qxk24_qiubbx_destroy(validator);
```

QIUBBX validates:

- Eight laws.
- Six directions.
- Seven levels.
- Kernel health.
- QXEngine health.
- QXMemloc health.
- SOVEREIGN score.

Use QIUBBX as a release proof, not as ordinary per-request runtime logic.

---

## 24. iOS HOWTO

The iOS bridge provides a Swift public API through `QXK24Engine`.

Core calls:

```swift
try QXK24Engine.shared.start()
try QXK24Engine.shared.registerAllGovernors()
try QXK24Engine.shared.cycle()

let health = QXK24Engine.shared.healthScore
let tier = QXK24Engine.shared.certTier
let report = try QXK24Engine.shared.generateReport()

QXK24Engine.shared.stop()
```

Observe health:

```swift
QXK24Engine.shared.onHealthChanged { snapshot in
    print("Health:", snapshot.score, snapshot.tier.label)
}
```

iOS lifecycle integration points:

- App launch: `start()` and `registerAllGovernors()`.
- Foreground: `cycle()`.
- Background: `cycle()` before background work.
- Heavy screen load: update or cycle after load.
- Upload/download: cycle before and after.
- Session end: `generateReport()` then `stop()`.

Suggested iOS policy:

```text
If tier is PROFESSIONAL or SOVEREIGN: continue.
If tier is STANDARD: reduce noncritical work.
If tier is UNCERTIFIED: stop risky work and preserve report.
```

---

## 25. Android HOWTO

The Android bridge provides a Kotlin public API through `QXK24Engine`.

Core calls:

```kotlin
QXK24Engine.start()
QXK24Engine.registerAllGovernors()
QXK24Engine.cycle()

val health = QXK24Engine.healthScore
val tier = QXK24Engine.certTier
val report = QXK24Engine.generateReport()

QXK24Engine.stop()
```

Observe health:

```kotlin
QXK24Engine.onHealthChanged = { snapshot ->
    println("Health: ${snapshot.score} ${snapshot.tier.label}")
}
```

Android lifecycle integration points:

- `Application.onCreate`: start and register governors.
- `Activity.onStart`: cycle.
- `Activity.onStop`: cycle before background.
- WorkManager job start/end: cycle and report.
- Upload/download: measure network and cycle.
- AI inference: cycle before and after.

Suggested Android policy:

```text
Use QXK24 cycle checkpoints around lifecycle and expensive work.
Do not call cycle in a tight UI loop.
Do not run unbounded background work without Fm/Ft policy.
```

---

## 26. Backend HOWTO

Recommended backend structure:

```text
server startup
  create kernel
  start kernel
  register resources

request middleware
  begin flow
  update input/network
  execute handler
  update CPU/storage/network
  complete flow
  cycle on configured boundary

health endpoint
  generate QXK24 report
  return tier and summary

shutdown
  final report
  stop and destroy kernel
```

Backend resource mapping:

| Backend Part | QXK24 Resource |
|---|---|
| Request body | Network/input |
| Handler CPU | CPU/execution |
| Database query | Storage |
| File read/write | I/O/storage |
| Response body | Network/output |
| Queue job | CPU/storage/network |

Release proof:

- Endpoint budgets are declared.
- Queue growth is measured.
- Database pressure affects report.
- Retry limits create correction.
- Critical failure creates stop behavior.

---

## 27. AI App HOWTO

AI apps should treat every inference as a governed flow.

AI mapping:

| AI Part | Direction | Resource |
|---|---|---|
| User prompt | `QXK24_DIR_KR` | Network/input/storage |
| Context retrieval | `QXK24_DIR_BG` | Storage/memory |
| Model inference | `QXK24_DIR_DN` | CPU/memory/energy |
| Tool call | Cross-flow | Network/I/O/storage |
| Response | `QXK24_DIR_KN` | Output/network |
| Memory write | `QXK24_DIR_BG` | Storage |

AI cycle pattern:

```text
measure prompt size
begin cross flow
retrieve context
run model
measure inference time and cost
complete tool calls
write memory
generate report
decide continue/correct/stop
```

Correction examples:

- Truncate context.
- Switch to smaller model.
- Disable noncritical tools.
- Delay memory writes.
- Ask user to confirm high-impact action.

Stop examples:

- Unsafe tool call.
- Health score below release threshold.
- Storage or memory budget breach.
- Unbounded loop detected.

---

## 28. Finance HOWTO

Finance apps should treat each transaction as cross-flow.

Transaction flow:

```text
input request
validate identity
begin cross flow
call payment/network
write ledger/storage
generate receipt/output
generate report
commit only if sound
```

Rules:

- Never commit critical state when report is unsound.
- Pair input and output.
- Preserve report for failed transactions.
- Treat duplicate retries as resource and flow risk.
- Use WAQF-style stop behavior for unsafe states.

Required proofs:

- Failed payment does not write success state.
- Duplicate retry is bounded.
- Ledger write is paired with report.
- Cross-flow produces health data.
- Unsafe state stops commit.

---

## 29. Healthcare HOWTO

Healthcare apps should focus on data integrity, privacy, controlled access, and
reporting without leaking private content.

Rules:

- Do not include private patient data in QXK24 report logs.
- Map data reads and writes as governed flows.
- Treat sync, export, and sharing as cross-flow.
- Stop unsafe data access.
- Preserve audit-friendly report summaries.

Required proofs:

- Access is directional.
- Report has no private payload.
- Sync failure is visible.
- Unsafe state requires intervention.
- Storage budget and lifecycle are controlled.

---

## 30. IoT HOWTO

IoT apps should focus on energy, I/O, offline storage, and network reconnects.

IoT cycle:

```text
sensor read
update I/O resource
write offline queue
update storage resource
attempt network send
update network resource
cycle
report device health
```

Rules:

- Bound sensor reads.
- Bound offline queue.
- Bound reconnect attempts.
- Use Ft for energy-sensitive work.
- Preserve compact reports for offline diagnostics.

Required proofs:

- Sensor rate limit works.
- Offline queue cannot grow forever.
- Reconnect does not loop forever.
- Energy report changes under load.
- Device can report degraded health.

---

## 31. Game and Interactive App HOWTO

Games and interactive apps should treat frame loops as cycles but should not run
heavy QXK24 reporting every frame unless designed for it.

Recommended pattern:

```text
light per-frame counters
periodic QXK24 resource update
cycle on scene load, asset load, network event, checkpoint, pause, exit
```

Map:

| Game Area | Resource |
|---|---|
| Frame update | CPU/energy |
| Asset loading | Storage/memory |
| Input | I/O/input |
| Rendering output | Output/energy |
| Multiplayer | Network |
| Save game | Storage |

Required proofs:

- Asset loading stays within memory budget.
- Network events are bounded.
- Save operation reports correctly.
- Health degrades when resource pressure is simulated.

---

## 32. Testing HOWTO

Treat tests as constitutional proofs.

Minimum test groups:

| Test Group | What It Proves |
|---|---|
| Lifecycle | Kernel create/start/cycle/stop/destroy works. |
| Resource | Registration, update, release, and budget handling work. |
| Flow | OAT/OASS begin/complete/cancel works. |
| Report | Reports generate and reflect state. |
| Health | Scores and tiers match expected thresholds. |
| Fm | Time cycles and snapshots work. |
| Ft | Energy cycles and snapshots work. |
| Caliph | Commands, reports, and judgments work. |
| Failure | Unsafe state creates correction or stop behavior. |
| Platform | iOS/Android bridges match app lifecycle. |

Example C++ test shape:

```cpp
TEST(AppGovernance, ReportsProfessionalAfterResourceRegistration) {
    QXK24Config cfg = qxk24_default_config();
    QXK24KernelHandle kernel = nullptr;
    ASSERT_EQ(QXK24_OK, qxk24_create(&cfg, &kernel));
    ASSERT_EQ(QXK24_OK, qxk24_start(kernel));

    uint32_t memory = 0;
    ASSERT_EQ(QXK24_OK, qxk24_resource_register(
        kernel,
        QXK24_RESOURCE_MEMORY,
        QXK24_DIR_AS,
        QXK24_LEVEL_SISTEM,
        256ULL * 1024ULL * 1024ULL,
        &memory
    ));

    ASSERT_EQ(QXK24_OK, qxk24_resource_update(kernel, memory, 64ULL * 1024ULL, 1.0));

    QXK24Report report;
    ASSERT_EQ(QXK24_OK, qxk24_cycle(kernel, &report));
    EXPECT_GE(report.health_score, QXK24_SCORE_STANDARD);

    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}
```

---

## 33. Release HOWTO

Before release, produce:

- Build log.
- Test count and pass rate.
- QXK24 report.
- Certification tier.
- Resource budget map.
- Known violations.
- Correction actions.
- Platform validation.
- Version and tag.

Release policy:

| State | Action |
|---|---|
| SOVEREIGN | Release allowed. |
| PROFESSIONAL | Release allowed for production if known risks are accepted. |
| STANDARD | Internal release or limited beta only. |
| NONE | Do not release. |
| Unsound report | Do not release until corrected. |

Recommended release artifact:

```text
RELEASE_PROOF.md
  version
  commit
  build result
  test result
  qxk24 health score
  qxk24 tier
  reports
  known risks
  decision
```

---

## 34. Runtime Monitoring HOWTO

QXK24 can work with existing observability tools.

Export:

- `health_score`.
- `cert_tier`.
- `cycle_index`.
- `active_resources`.
- `active_flows`.
- `global_utilisation`.
- `law_scores`.
- resource utilization.
- correction and stop decisions.

Do not export private payloads through QXK24 reports. Export summary data and
safe identifiers only.

---

## 35. Troubleshooting

### Kernel Create Fails

Check:

- Config pointer is valid.
- Output handle pointer is valid.
- Memory allocation is available.

### Start Fails

Check:

- Kernel handle is not null.
- Kernel is not already in invalid state.

### Resource Registration Fails

Check:

- Kernel is started.
- Resource type is valid.
- Direction is valid.
- Level is valid.
- Soft budget is nonzero.
- Resource capacity has not exceeded `QXK24_MAX_RESOURCES`.

### Health Is Too Low

Check:

- Resources are registered.
- Usage is updated.
- `x_measured` is realistic.
- Directions are paired.
- Flows are completed or canceled.
- Budgets are not exceeded.
- Reports are generated after meaningful updates.

### Active Flows Never Decrease

Check:

- Every `qxk24_flow_begin()` has `complete()` or `cancel()`.
- Every OAT/OASS flow is completed or canceled.
- Error paths clean up flow IDs.

### Mobile Bridge Shows Not Running

Check:

- `start()` was called before `cycle()`.
- Native library is linked.
- JNI or Objective-C bridge is loaded.
- App lifecycle did not call `stop()` earlier than expected.

---

## 36. Practical App Template

Use this structure for most apps:

```text
src/
  governance/
    AppGovernance.c
    AppGovernance.h
    ResourceMap.c
    ResourceMap.h
    HealthPolicy.c
    HealthPolicy.h

docs/
  APP_CATEGORY.md
  DIRECTION_MAP.md
  RESOURCE_BUDGETS.md
  RELEASE_PROOF.md

tests/
  governance/
    test_lifecycle.cpp
    test_resources.cpp
    test_report.cpp
    test_failure_policy.cpp
```

`AppGovernance` owns QXK24. Feature modules call `AppGovernance`, not raw
kernel functions, unless they are low-level infrastructure modules.

---

## 37. Complete Integration Checklist

Before coding:

- [ ] App category is declared.
- [ ] Six directions are mapped.
- [ ] K24 levels are assigned.
- [ ] Resource budgets are defined.
- [ ] Cycle schedule is defined.
- [ ] Health policy is defined.

During coding:

- [ ] Kernel lifecycle is wrapped.
- [ ] Resources are registered.
- [ ] Resource usage is updated.
- [ ] Direct flows use OAT or kernel flow APIs.
- [ ] Cross flows use OASS or kernel flow APIs.
- [ ] Reports are generated.
- [ ] Correction and stop actions exist.

Before release:

- [ ] Build passes.
- [ ] Tests pass.
- [ ] QXK24 report is generated.
- [ ] Health score meets target.
- [ ] Certification tier meets target.
- [ ] QIUBBX validation runs when SOVEREIGN proof is required.
- [ ] Release proof is stored.

---

## 38. Final Developer Rule

Do not treat QXK24 as decoration. Treat it as the app's constitutional
governance layer.

If a feature consumes resources, it should be measured.
If a flow crosses boundaries, it should be tracked.
If a process runs over time, it should be bounded.
If a system becomes unhealthy, it should correct or stop.
If an app is released, it should carry proof.

That is the practical meaning of building apps with QXK24.

---

**ALAMTOLOGI - Quranic Science | Founder: Masa Bayu**
