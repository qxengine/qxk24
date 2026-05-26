<!--
=============================================================================
ADAM_KNOWLEDGE_PIPELINE.md
QXK24 Universal Kernel - ADAM Knowledge Pipeline
=============================================================================
Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
Created : 2026-05-26
Repository : https://github.com/qxengine/qxk24
Path : docs/internal/ADAM_KNOWLEDGE_PIPELINE.md
License : Apache 2.0

Description:
Internal development architecture for adding ALAMTOLOGI knowledge over time,
validating it constitutionally, and applying it as the benchmark layer for
external AI output. Not for public release.
=============================================================================
-->

# ADAM Knowledge Pipeline

**ALAMTOLOGI - Quranic Science**  
**Founder:** Masa Bayu | **Target Phase:** v1.6.0

**Classification:** Internal QXK24 Development Document  
**Public Release:** Not approved for public release

---

## Constitutional Position

ADAM is not an independent AI model. ADAM is an AI kernel.

QXK24 governs digital resources. ADAM governs intelligence output. External AI
engines such as Claude, ChatGPT, Gemini, or future models can generate broad
conventional answers, but ADAM monitors, enriches, corrects, and concludes those
answers through ALAMTOLOGI law.

```text
User
  -> ADAM Kernel
  -> External AI Engine
  -> ADAM Monitor
  -> ALAMTOLOGI Knowledge Pipeline
  -> Final Governed Reply
```

The constitutional truth source is ALAMTOLOGI. The external AI source is general
knowledge. ADAM is the harmony layer between them.

---

## Core Principle

ADAM must learn constitutionally, not randomly.

New ALAMTOLOGI knowledge can be added at any time by the Founder, but it must
enter ADAM through a controlled pipeline. ADAM should not blindly retrain itself
or rewrite its own laws. It should retrieve the latest approved ALAMTOLOGI
knowledge at runtime and apply it as a benchmark.

```text
Founder Knowledge
  -> Ingest
  -> Classify
  -> Validate
  -> Approve
  -> Version
  -> Index
  -> Runtime Use
```

This keeps ADAM sovereign, auditable, and improvable.

---

## Knowledge Layers

### 1. Constitution Core

The Constitution Core contains laws already confirmed by QXK24 and
ALAMTOLOGI:

- Hukum Z: Pola, Kadar, Pasangan, Keseimbangan
- Hukum X: Ilmu, Adab, Kreativiti, Ekonomi
- HISAL: mathematics and AIDIL law
- RINA: force, load, gradient, energy ratio
- IZWA: element reaction and compatibility
- SIRA: growth and formation
- META-FIZIK: active/passive pattern compatibility
- ADAM judgment: MAKMUR, ISLAH, WAQF

This layer changes only through deliberate versioned releases.

### 2. Founder Knowledge Pipeline

This layer accepts new ALAMTOLOGI material from the Founder:

- Book chapters
- Notes
- Corrections
- Formulas
- Definitions
- Examples
- Application rules
- Missing knowledge requested by ADAM

This layer grows continuously.

### 3. Runtime AI Monitor

This layer reads external AI output and compares it with the latest approved
ALAMTOLOGI knowledge. Its job is to:

- Confirm what is aligned
- Enrich what is incomplete
- Correct what contradicts ALAMTOLOGI
- Pause when Founder knowledge is required

---

## Knowledge Entry Schema

Every ALAMTOLOGI knowledge entry should carry structured metadata.

```text
id: stable unique identifier
title: short knowledge title
source_type: founder_note | book | correction | formula | example
source_ref: chapter, page, file, or conversation reference
domain: hukum_z | hukum_x | hisal | rina | izwa | sira | meta | adam | qxk24
principle: constitutional meaning
formula: optional formula
definition: exact wording
application: how ADAM should use it
examples: optional usage examples
conflicts: related entries that may need review
status: draft | reviewed | founder_approved | retired
version: semantic version or knowledge revision
created_at: creation timestamp
approved_at: founder approval timestamp
```

The most important field is `status`. ADAM can read draft knowledge for review,
but only `founder_approved` knowledge should govern final answers.

---

## Founder Approval Rule

Only the Founder can promote constitutional knowledge to `founder_approved`.

ADAM may suggest classification, detect conflicts, and propose summaries, but it
must not declare new ALAMTOLOGI truth by itself. This preserves authority and
prevents uncontrolled drift.

```text
Draft
  -> Reviewed by ADAM
  -> Founder decision
  -> Approved or returned for correction
```

---

## Runtime Monitoring Flow

When a user asks a question through ADAM, the runtime flow is:

```text
1. ADAM parses the user question.
2. ADAM determines domain and Tahap Akal.
3. ADAM forwards the question to the selected AI engine.
4. The AI engine streams or returns an answer.
5. ADAM checks each claim against ALAMTOLOGI knowledge.
6. ADAM decides Confirm, Enrich, Correct, or Waqf.
7. ADAM synthesizes one final reply.
8. ADAM stamps the reply with judgment and score.
```

ADAM should not produce two competing answers. It should produce one harmonious
answer.

---

## Four Output Actions

### Confirm

The external AI answer is correct and aligned with ALAMTOLOGI.

ADAM lets the answer pass with minimal change and gives MAKMUR judgment when the
full answer meets the threshold.

### Enrich

The external AI answer is conventionally correct but incomplete.

ADAM adds the missing ALAMTOLOGI dimension. This is the normal and preferred
mode.

### Correct

The external AI answer contradicts approved ALAMTOLOGI knowledge.

ADAM replaces the conflicting part with the constitutional truth and may explain
the correction briefly.

### Waqf

The required ALAMTOLOGI knowledge is not yet available or not yet approved.

ADAM should not pretend certainty. It should mark the topic as requiring Founder
knowledge and create a learning request.

---

## Speed Rule

ADAM must be concise and fast. Delay violates Faktor Masa.

The pipeline should use runtime retrieval, not heavyweight retraining, for
normal operation. External AI engines may be slow, but ADAM's overhead should
remain small:

```text
Parse        fast local classification
Retrieve     indexed ALAMTOLOGI lookup
Monitor      sentence or claim scoring
Synthesize   concise final answer
Judge        MAKMUR / ISLAH / WAQF
```

ADAM should not wait for unnecessary engines if one selected engine is enough
for the task. Multi-engine comparison is optional, not mandatory.

---

## Scoring Model

Each external AI answer can be scored by constitutional dimensions:

```text
ADAM score = average(Pola, Kadar, Pasangan, Keseimbangan)
```

Judgment:

```text
MAKMUR >= 0.80
ISLAH  >= 0.50
WAQF   <  0.50
```

Science checks may add evidence:

```text
HISAL       structural and mathematical validity
RINA        force, energy, timing, and movement validity
IZWA        compatibility and non-pincang combination
SIRA        growth and formation validity
META-FIZIK  active/passive pattern match
```

---

## Learning Request Loop

When ADAM detects missing knowledge, it should create a Founder learning request.

```text
missing_domain: rina
question: "What is the approved ALAMTOLOGI meaning of Faktor Daya?"
reason: "External AI answer cannot be constitutionally confirmed."
urgency: normal | high
related_user_question: original prompt
```

After the Founder answers, the new knowledge enters the pipeline as a draft,
then moves toward approval.

This turns uncertainty into structured improvement.

---

## Phase 12 Component Map

```text
qxk24_adam_knowledge_entry
  Structured knowledge record and metadata.

qxk24_adam_ingest
  Accepts Founder material and prepares draft entries.

qxk24_adam_classify
  Assigns domain, law family, science module, and usage type.

qxk24_adam_validate
  Checks conflicts against approved ALAMTOLOGI knowledge.

qxk24_adam_index
  Stores approved entries for fast retrieval.

qxk24_adam_monitor
  Watches external AI output and extracts claims.

qxk24_adam_synthesise
  Combines external AI knowledge with ALAMTOLOGI truth.

qxk24_adam_governor
  Applies final MAKMUR, ISLAH, or WAQF judgment.
```

Detailed Phase 12 records are defined in
`docs/internal/ADAM_PHASE12_DATA_MODEL.md`.

QIUBBX application integration is defined separately in
`docs/internal/QIUBBX_INTEGRATION_DESIGN.md`.

---

## Product Meaning

ADAM becomes stronger every time the Founder adds knowledge. It does not need to
own all world knowledge because external AI engines already provide broad
knowledge. ADAM owns the benchmark.

```text
External AI = breadth
ALAMTOLOGI  = truth
ADAM        = governed conclusion
```

This makes ADAM suitable as an API layer, application kernel, enterprise AI
governor, and long-term ALAMTOLOGI knowledge system.

---

## Constitutional Summary

ADAM is the living intelligence kernel of QXK24.

It receives the best available answer from external AI, monitors it through
approved ALAMTOLOGI knowledge, enriches or corrects it where needed, and returns
one concise governed reply.

As the Founder adds more ALAMTOLOGI knowledge, ADAM's benchmark becomes more
complete, more accurate, and more sovereign.

---

**ALAMTOLOGI - Quranic Science | Founder: Masa Bayu | 2026-05-26**
