<!--
=============================================================================
QIUBBX_INTEGRATION_DESIGN.md
QXK24 Universal Kernel - QIUBBX Integration Design
=============================================================================
Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
Created : 2026-05-26
Repository : https://github.com/qxengine/qxk24
Path : docs/internal/QIUBBX_INTEGRATION_DESIGN.md
License : Apache 2.0

Description:
Internal development design for wiring QXK24 into QIUBBX through the
QIUBBX Constitutional Adapter. Not for public release.
=============================================================================
-->

# QIUBBX Integration Design

**ALAMTOLOGI - Quranic Science**  
**Founder:** Masa Bayu | **Target:** QIUBBX Pilot Integration

**Classification:** Internal QXK24 Development Document  
**Public Release:** Not approved for public release

---

## Purpose

This document defines how QIUBBX should integrate with QXK24 safely.

QXK24 is the constitutional kernel. QIUBBX is the application. QIUBBX should not
call every QXK24 module directly. It should call one controlled application
protocol: the QIUBBX Constitutional Adapter.

```text
QIUBBX App / Backend
  -> QIUBBX Constitutional Adapter
  -> QXK24 Kernel
  -> QXEngine / Caliph / ADAM / Science / FT / FM
  -> Governed application decision
```

The adapter returns simple decisions:

```text
ALLOW
WARN
REVIEW
BLOCK
```

---

## Public vs Internal

This document is internal because it contains:

- QIUBBX production flow structure
- Risk and trust thresholds
- Decision mapping
- Audit log design
- Future platform wiring
- Commercial application strategy

Only simplified API documentation should become public later.

---

## Integration Principle

QIUBBX must remain application-focused. QXK24 must remain constitutional.

The adapter translates between them:

```text
QIUBBX language:
  user action, listing, payment, transaction, dispute

QXK24 language:
  law, health, pattern, limit, pair, equilibrium, judgment
```

QIUBBX should not need to understand ADAM, HISAL, RINA, IZWA, SIRA, META, FT, FM,
or Caliph directly for the first pilot. Those modules remain behind the adapter.

---

## Runtime Placement

Recommended first deployment:

```text
QIUBBX Backend Service
  -> Native QXK24 library
  -> qxk24_qiubbx_adapter
```

Reason:

- Backend has stable runtime control.
- Backend can store audit logs safely.
- Backend can enforce decisions before payment, listing, or dispute state
  changes.
- Mobile apps can receive decisions without embedding the full policy layer.

Mobile integration can follow after backend proof.

---

## Adapter Lifecycle

The service should create one adapter per service process or worker.

```text
1. Create QXK24 kernel.
2. Start QXK24 kernel.
3. Create QIUBBX adapter with kernel handle.
4. Govern application events.
5. Periodically read constitutional report.
6. Destroy adapter.
7. Stop and destroy kernel.
```

The adapter must not own private user data beyond the active decision call.

---

## Application Flows

### User Action

Used for login, profile update, message send, follow, review, checkout start,
merchant action, or admin action.

Input:

```text
user_id
action
risk_score
has_consent
requires_review
```

Decision rules:

- Missing consent -> `BLOCK`
- Extreme risk -> `BLOCK`
- High risk or flagged review -> `REVIEW`
- Low risk with consent -> `ALLOW`

First QIUBBX use:

```text
govern_user_action(checkout_start)
```

### Transaction

Used when buyer and seller enter a commerce transaction.

Input:

```text
transaction_id
amount
buyer_trust
seller_trust
inventory_available
payment_authorized
```

Decision rules:

- Invalid amount -> `BLOCK`
- Payment not authorized -> `BLOCK`
- Inventory unavailable -> `BLOCK`
- Low trust -> `REVIEW`
- High trust and valid state -> `ALLOW`

First QIUBBX use:

```text
govern_transaction(before_order_create)
```

### Listing

Used when merchant creates or updates a product/service listing.

Input:

```text
listing_id
category
price
stock_quantity
truthful_description
prohibited_item
```

Decision rules:

- Prohibited item -> `BLOCK`
- Invalid price -> `BLOCK`
- Zero stock -> `BLOCK`
- Untruthful or incomplete description -> `REVIEW`
- Truthful stocked listing -> `ALLOW`

First QIUBBX use:

```text
govern_listing(before_publish)
```

### Payment

Used before capture, transfer, refund, or payout.

Input:

```text
payment_id
amount
fraud_score
authorized
balance_available
```

Decision rules:

- Invalid amount -> `BLOCK`
- Unauthorized payment -> `BLOCK`
- Insufficient balance -> `BLOCK`
- Extreme fraud risk -> `BLOCK`
- Medium fraud risk -> `REVIEW`
- Low risk authorized payment -> `ALLOW`

First QIUBBX use:

```text
govern_payment(before_capture)
```

### Dispute

Used when buyer, seller, admin, or system opens a dispute.

Input:

```text
dispute_id
evidence_count
severity
buyer_claim_present
seller_response_present
```

Decision rules:

- No buyer claim -> `BLOCK`
- Missing seller response -> `REVIEW`
- High severity -> `REVIEW`
- Evidence present from both sides -> `ALLOW` or continue normal process

First QIUBBX use:

```text
govern_dispute(before_resolution)
```

---

## Decision Contract

Every adapter call returns:

```text
flow
decision
constitutional_score
kernel_health
severity
reason
```

QIUBBX should map decisions as follows:

```text
ALLOW   continue immediately
WARN    continue but show warning or increase monitoring
REVIEW  stop automation and route to human/system review
BLOCK   reject operation and store audit evidence
```

`BLOCK` should always be auditable. `REVIEW` should always be explainable.

---

## Audit Log

Every decision must be logged by QIUBBX, not only by QXK24.

Minimum audit event:

```text
event_id
timestamp
qiubbx_flow
subject_id
decision
constitutional_score
kernel_health
severity
reason
input_hash
qxk24_version
adapter_version
request_id
```

Rules:

- Do not store full payment credentials.
- Do not store raw private user content unless required by law or dispute.
- Store hashes or references where possible.
- Keep enough evidence to explain `REVIEW` and `BLOCK`.

---

## API Shape for Backend

Recommended service-level API:

```text
POST /constitutional/user-action
POST /constitutional/transaction
POST /constitutional/listing
POST /constitutional/payment
POST /constitutional/dispute
GET  /constitutional/report
```

Response shape:

```json
{
  "decision": "ALLOW",
  "flow": "TRANSACTION",
  "constitutionalScore": 0.94,
  "kernelHealth": 99.5,
  "severity": 0,
  "reason": "Transaction passed QIUBBX constitutional checks",
  "requestId": "..."
}
```

This JSON API should wrap the native C adapter.

---

## First Pilot

The first pilot should be transaction governance.

Why:

- Transaction is central to QIUBBX.
- It touches buyer, seller, stock, payment, and trust.
- It produces measurable results.
- It can run in shadow mode before enforcement.

Pilot mode:

```text
Phase A: Shadow only
  QXK24 gives decision, QIUBBX logs it, but does not block.

Phase B: Review enforcement
  REVIEW and BLOCK are routed to internal review, but customer flow is protected.

Phase C: Production enforcement
  BLOCK stops invalid transactions automatically.
```

---

## Shadow Mode

Before production enforcement, run QXK24 in shadow mode.

In shadow mode:

- QIUBBX calls adapter.
- Decision is logged.
- Existing QIUBBX behavior continues.
- Reports are reviewed daily.
- Thresholds are adjusted before enforcement.

Shadow mode protects users while proving the law.

---

## Platform Bridge Plan

### Backend

Primary integration path. Native C/C++ binding or service wrapper.

### iOS

After backend proof:

- Swift wrapper for decision display
- Local pre-checks for user action
- No sensitive payment decision stored on device

### Android

After backend proof:

- Kotlin wrapper for decision display
- Local pre-checks for user action
- Backend remains final authority

---

## ADAM Role

ADAM should not block the first integration.

Initial adapter decisions are deterministic. ADAM can be added later for:

- Human-readable explanations
- Founder-approved policy enrichment
- Dispute explanation
- Merchant education
- Support agent guidance

ADAM should explain; the adapter should decide.

---

## Science Engine Role

Science modules can strengthen future versions:

```text
HISAL  amount, quantity, and segmentation validity
RINA   load, flow, risk, and timing balance
IZWA   buyer/seller/listing compatibility
SIRA   user/merchant lifecycle growth
META   pattern compatibility and abnormal behavior
```

First pilot can run without deep science coupling. Later phases can enrich the
adapter score.

---

## Production Readiness Checklist

Before full QIUBBX enforcement:

- QIUBBX backend wrapper implemented
- JSON response contract locked in adapter
- Audit event protocol implemented in adapter
- Shadow mode enabled
- Transaction pilot complete
- Performance benchmark complete
- Concurrency test complete
- Privacy review complete
- Security review complete
- Rollback flag implemented
- Human review queue connected
- Daily constitutional report generated

---

Implementation status and rollout notes are tracked in
`docs/internal/QIUBBX_INTEGRATION_STATUS.md`.

---

**ALAMTOLOGI - Quranic Science | Founder: Masa Bayu | 2026-05-26**
