# QXK24 Developer SOP
## Standard Operating Procedure for Any App Category

**ALAMTOLOGI – Quranic Science**  
**Founder:** Masa Bayu  
**Repository:** https://github.com/qxk24/qxk24  
**Current Version:** v1.3.0  
**Companion Paper:** `docs/WHITEPAPER.md`

---

## Purpose

This SOP gives developers a repeatable procedure for building any category of application with QXK24. It converts the QXK24 constitutional model into daily engineering steps: define the app, map resources, measure work, run cycles, generate reports, judge health, test proofs, and release only when the system can explain its own state.

The SOP applies to mobile apps, backend APIs, AI systems, financial software, healthcare tools, education platforms, IoT software, games, developer tools, and operating utilities.

---

## Core Rule

Every application must be able to answer:

```text
What exists?
Where does it move?
How is it measured?
How much does it consume?
What is its health?
What should happen next?
```

If an application cannot answer these questions, it is not yet
constitutionally ready.

---

## Step 1: Declare the App Category

Before coding, define the category of the app. Examples:

- Mobile consumer app.
- Enterprise dashboard.
- AI assistant.
- Financial app.
- Education app.
- Healthcare app.
- IoT controller.
- Game.
- Developer tool.
- Backend API.

Then define the app's primary constitutional load.

| App Category | Main Load |
|---|---|
| Mobile app | Memory, energy, network |
| Backend API | CPU, network, storage |
| AI app | Memory, CPU, energy, data flow |
| Finance | Storage, network, audit flow |
| Healthcare | Data integrity, privacy, reports |
| IoT | Energy, I/O, network |
| Game | CPU, memory, graphics, input |
| Education | Storage, network, progress cycles |

Deliverable:

```text
APP_CATEGORY.md
```

It should state the app category, main resource risks, and minimum health tier.

---

## Step 2: Map the Six Directions

Every application should map its resources into six directions.

| Direction | Code | Developer Meaning |
|---|---|---|
| Top | As | Governance, policy, orchestration |
| Bottom | Bh | Hardware, platform, device limits |
| Front | Dn | Execution, user action, runtime work |
| Back | Bg | Storage, history, persistence |
| Right | Kn | Output, response, egress |
| Left | Kr | Input, request, ingress |

Example for an AI assistant:

- `As`: safety policy, system rules, model orchestration.
- `Bh`: device or server runtime.
- `Dn`: inference execution.
- `Bg`: conversation memory and vector storage.
- `Kn`: answer output.
- `Kr`: prompt input.

Deliverable:

```text
DIRECTION_MAP.md
```

It should list each direction and which app modules belong there.

---

## Step 3: Define the K24 Level

Choose the K24 level based on component size and responsibility.

| Component Type | Suggested Level |
|---|---|
| Primitive data unit | K24za |
| Small helper or value object | K24ji |
| Atomic service or locator | K24at |
| Internal subsystem | K24si |
| Governor or major object | K24ob |
| Runtime service | K24ak |
| App master module | K24md |

Do not make every component a master. Most features should be lower-level
systems governed by one master kernel.

Deliverable:

```text
K24_LEVEL_MAP.md
```

---

## Step 4: Register Resources

Each major resource must be registered with:

- Type.
- Direction.
- Level.
- Soft budget.
- Hard budget.
- Measured usage.

Minimum resources to consider:

- Memory.
- CPU.
- Network.
- Storage.
- I/O.
- Energy.

Developer rule:

```text
No resource should be consumed without ownership and measurement.
```

---

## Step 5: Start the Constitutional Cycle

Every QXK24-governed application should follow this cycle:

```text
create kernel
start kernel
register resources
cycle
measure health
generate report
judge result
continue, correct, or stop
```

Run the cycle:

- At startup.
- After major actions.
- Before uploads or downloads.
- Before payment or transaction commit.
- Before AI inference.
- Before sync.
- Before session end.

---

## Step 6: Add Faktor Masa

Faktor Masa makes time explicit. Use it to define:

- When a process begins.
- Which controlled time unit it occupies.
- How many cycles have passed.
- Whether the app is still within its time boundary.

Developer rule:

```text
No background work should run without a time boundary.
```

---

## Step 7: Add Faktor Tenaga

Faktor Tenaga makes energy explicit. Use it to define:

- Active and passive energy.
- Energy phase.
- Energy movement.
- Flow count.
- Rotation stability.
- No-overlap behavior.

Developer rule:

```text
No repeated work, duplicate flow, or energy-heavy process should be invisible.
```

---

## Step 8: Add Caliph Judgment

The Caliph layer represents stewardship above the kernel. In application terms,
this is the responsible controller that decides what to do with kernel reports.

Judgment codes:

| Judgment | Meaning | Developer Action |
|---|---|---|
| MAKMUR | Healthy and sound | Continue |
| ISLAH | Needs correction | Reduce load, retry, repair, rebalance |
| WAQF | Breach or unsafe | Stop, fail closed, require intervention |

Developer rule:

```text
Never ignore ISLAH or WAQF.
```

---

## Step 9: Write Tests as Proofs

Every application category should include tests for:

- Creation and startup.
- Resource registration.
- Resource updates.
- Health scoring.
- Report generation.
- Failure handling.
- Cycle stability.
- Platform lifecycle.
- Category-specific risks.

Treat tests as constitutional proofs, not only code coverage.

Minimum test groups:

```text
Core lifecycle tests
Resource governance tests
Flow tests
Time tests
Energy tests
Report tests
Failure and recovery tests
Platform tests
```

---

## Step 10: Release Only with a Report

Before release, every app should produce:

- Build result.
- Test count.
- Health score.
- Certification tier.
- Known risks.
- Platform validation result.
- Release tag.

An application is not constitutionally ready if it cannot report its own state.

---

## SOP by App Category

### Mobile Apps

Focus on memory, energy, lifecycle, and network.

Required proof:

- No uncontrolled memory growth.
- Background work is bounded.
- Network retries are measured.
- Battery-heavy work is visible.
- Reports remain PROFESSIONAL or SOVEREIGN under normal usage.

### Backend APIs

Focus on CPU, network, storage, and flow direction.

Required proof:

- Each endpoint has resource budgets.
- Queue growth is measured.
- Database load is reported.
- Rate limits map to constitutional limits.
- Health check includes QXK24 report status.

### AI Applications

Focus on memory, CPU, prompt flow, model inference, and storage.

Required proof:

- Prompt size is measured.
- Inference cost is measured.
- Memory use is governed.
- Unsafe or failed inference creates ISLAH or WAQF.
- User-facing output remains simple even when internal governance is deep.

### Financial Apps

Focus on auditability, storage, network, and correctness.

Required proof:

- Inputs and outputs are paired.
- Writes are auditable.
- Failed transactions do not continue silently.
- WAQF is used for unsafe states.
- Reports are preserved for review.

### Healthcare Apps

Focus on data integrity, privacy, storage, and controlled access.

Required proof:

- Data access is governed.
- Reports expose risk without exposing private data.
- Storage and network flows are directional.
- Unsafe state stops or asks for human intervention.

### IoT and Device Apps

Focus on energy, I/O, network, and offline storage.

Required proof:

- Sensor reads are bounded.
- Offline queues are measured.
- Energy usage is visible.
- Network reconnect behavior is governed.

### Games and Interactive Apps

Focus on CPU, memory, input, output, and frame stability.

Required proof:

- Frame cycle is measured.
- Memory spikes are governed.
- Input and output are paired.
- Asset loading has storage and memory budgets.

---

## Minimum Developer Checklist

Before building:

- Define app category.
- Define six directions.
- Define K24 level per major component.
- Define resource budgets.
- Define cycle timing.
- Define report thresholds.

During development:

- Register all major resources.
- Update measurements after meaningful work.
- Generate reports during lifecycle events.
- Treat ISLAH as required correction.
- Treat WAQF as a hard stop.

Before release:

- Build passes.
- Tests pass.
- Line limits pass.
- Health score meets target.
- Reports are generated.
- Platform validation is complete.
- Version and tag are correct.

---

## Final SOP Statement

QXK24 development is not only feature delivery. It is governed delivery. Every
application should be built so that its resources are declared, its flows are
measured, its time is bounded, its energy is visible, its reports are readable,
and its failures produce correction instead of silence.

```text
Deep inside. Simple outside.
```

---

**ALAMTOLOGI – Quranic Science | Founder: Masa Bayu**
