<!--
=============================================================================
QIUBBX_INTEGRATION_STATUS.md
QXK24 Universal Kernel - QIUBBX Integration Status
=============================================================================
Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
Created : 2026-05-26
Repository : https://github.com/qxengine/qxk24
Path : docs/internal/QIUBBX_INTEGRATION_STATUS.md
License : Apache 2.0

Description:
Internal implementation status for the QIUBBX Constitutional Adapter.
Not for public release.
=============================================================================
-->

# QIUBBX Integration Status

**Classification:** Internal QXK24 Development Document  
**Public Release:** Not approved for public release

---

## Implemented In QXK24

- QIUBBX adapter C ABI
- User action, transaction, listing, payment, and dispute governance
- Constitutional adapter report
- Runtime modes: `shadow`, `review_only`, `enforce`
- Recommended vs effective decision tracking
- Audit event creation
- Bounded in-kernel audit ledger for recent decisions
- Mutex-protected adapter state for backend worker concurrency
- JSON decision serialization
- Audit event JSON serialization for backend persistence handoff
- Backend transaction pilot DTO mapping
- Unit tests for all QIUBBX adapter flows

Verified:

```text
QXK24 CTest: 1087/1087
```

---

## Rollback Strategy

Every enforcement path must have a rollback flag.

```text
QXK24_MODE=shadow
QXK24_MODE=review_only
QXK24_MODE=enforce
```

If adapter rules become too strict:

- Switch to `review_only` or `shadow`
- Preserve logs
- Review blocked decisions
- Adjust thresholds
- Re-enable enforcement

---

## Not Yet Implemented

- Production traffic review over real customer checkout volume
- iOS/Android UI wrappers

---

## Implemented In QIUBBX Backend

- REST wrapper mounted at `/api/constitutional`
- Transaction pilot endpoint: `POST /api/constitutional/transaction`
- Constitutional report endpoint: `GET /api/constitutional/report`
- Audit lookup endpoint: `GET /api/constitutional/audits/:requestId`
- MongoDB audit persistence model: `QXK24AuditEvent`
- Trust mapping from explicit trust scores or order history
- Fraud score normalization
- Shadow-mode effective decision with preserved recommended decision
- Native Node N-API addon linked to QXK24 C adapter
- Native bridge loader with contract fallback
- Optional install-time native build guard
- QXK24 Shadow Proof wired into live checkout before payment/order creation
- Shadow Proof report endpoint: `/api/constitutional/shadow-proof/report`
- Audit review endpoint: `/api/constitutional/audits/:eventId/review`
- Review-only readiness endpoint: `/api/constitutional/shadow-proof/readiness`
- Native Shadow Proof benchmark script: `npm run benchmark:qxk24-shadow`
- Focused backend contract test
- Production Linux native bridge build
- Production native bridge required mode
- Production QXK24 enforcement mode

Verified:

```text
QIUBBX backend native smoke: passed
QIUBBX backend native benchmark: passed
QIUBBX backend Jest: 14/14
QIUBBX backend typecheck: passed
QIUBBX production health: healthy
QIUBBX production native bridge: required
QIUBBX production mode: enforce
```

---

## Production Runtime Stage

QXK24 has moved from concept and test kernel into the live QIUBBX
production runtime.

Production state:

```text
QXK24_MODE=enforce
QXK24_NATIVE_BRIDGE=required
Bridge source: native
Native binary: Linux ELF x86-64
QXK24 core: built on production Linux server
```

Completed progression:

```text
1. Create QIUBBX transaction request DTO.          DONE
2. Map DTO to QXK24 QXK24QIUBBXTransaction.       DONE
3. Call QXK24 QIUBBX transaction governance.      DONE
4. Return JSON decision.                          DONE
5. Store audit event.                             DONE
6. Run development Shadow Proof checkout batch.   DONE
7. Review audit outcomes.                         DONE
8. Pass readiness gate for review_only.           DONE
9. Move production to review_only.                DONE
10. Rebuild native bridge on production Linux.    DONE
11. Require native bridge in production.          DONE
12. Move production to enforce.                   DONE
```

Development Shadow Proof result:

```text
Checkout attempts: 100
Orders created: 100
Audit events created: 100
Review outcomes: accepted 88, true_positive 12
Readiness gate: move_to_review_only
```

Production enforce smoke result:

```text
Normal transaction: WARN
Risky transaction: BLOCK
Bridge source: native
Mode: enforce
```

The live checkout path calls QXK24 before payment/order creation. In `enforce`
mode, QXK24 is the active native gatekeeper for QIUBBX checkout governance.

Daily review endpoint:

```text
GET /api/constitutional/shadow-proof/report?hours=24
GET /api/constitutional/shadow-proof/readiness?hours=168&minSamples=100
PATCH /api/constitutional/audits/:eventId/review
```

---

**ALAMTOLOGI - Quranic Science | Founder: Masa Bayu | 2026-05-26**
