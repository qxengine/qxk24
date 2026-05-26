/* ============================================================
 * QXK24Tests.swift
 * QXK24 Universal Kernel - iOS Swift Bridge Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/platform/ios/QXK24Tests.swift
 * License : Apache 2.0
 *
 * Description:
 * Provides 30 Phase 4 proofs for the Swift platform bridge.
 * ============================================================ */

import XCTest
@testable import QXK24

final class QXK24Tests: XCTestCase {
    override func setUp() {
        QXK24Engine.shared.stop()
    }

    override func tearDown() {
        QXK24Engine.shared.stop()
    }

    func test_version_isAlpha4() {
        XCTAssertEqual(QXK24_VERSION, "1.0.0-alpha.4")
    }
    func test_founder_isMasaBayu() {
        XCTAssertEqual(QXK24_FOUNDER, "Masa Bayu")
    }
    func test_science_containsAlamtologi() {
        XCTAssertTrue(QXK24_SCIENCE.contains("ALAMTOLOGI"))
    }
    func test_certTier_sovereign_threshold() {
        XCTAssertEqual(QXK24CertTier.sovereign.threshold, 95.0)
    }
    func test_certTier_professional_threshold() {
        XCTAssertEqual(QXK24CertTier.professional.threshold, 80.0)
    }
    func test_certTier_standard_label() {
        XCTAssertEqual(QXK24CertTier.standard.label, "STANDARD")
    }
    func test_certTier_uncertified_label() {
        XCTAssertEqual(QXK24CertTier.uncertified.label, "UNCERTIFIED")
    }
    func test_direction_as_role() {
        XCTAssertEqual(QXK24Direction.as_.role, "Governance")
    }
    func test_direction_bh_role() {
        XCTAssertEqual(QXK24Direction.bh.role, "Hardware")
    }
    func test_direction_as_pair_is_bh() {
        XCTAssertEqual(QXK24Direction.as_.pair, .bh)
    }
    func test_direction_kn_pair_is_kr() {
        XCTAssertEqual(QXK24Direction.kn.pair, .kr)
    }
    func test_direction_dn_pair_is_bg() {
        XCTAssertEqual(QXK24Direction.dn.pair, .bg)
    }
    func test_engine_start_succeeds() throws {
        XCTAssertNoThrow(try QXK24Engine.shared.start())
    }
    func test_engine_isRunning_afterStart() throws {
        try QXK24Engine.shared.start()
        XCTAssertTrue(QXK24Engine.shared.isRunning)
    }
    func test_engine_isNotRunning_afterStop() throws {
        try QXK24Engine.shared.start()
        QXK24Engine.shared.stop()
        XCTAssertFalse(QXK24Engine.shared.isRunning)
    }
    func test_activeResources_startsAtZero() throws {
        try QXK24Engine.shared.start()
        XCTAssertEqual(QXK24Engine.shared.activeResources, 0)
    }
    func test_cycleIndex_startsAtZero() throws {
        try QXK24Engine.shared.start()
        XCTAssertEqual(QXK24Engine.shared.cycleIndex, 0)
    }
    func test_cycleIndex_incrementsAfterCycle() throws {
        try QXK24Engine.shared.start()
        try QXK24Engine.shared.cycle()
        XCTAssertEqual(QXK24Engine.shared.cycleIndex, 1)
    }
    func test_cycle_throwsWhenStopped() {
        XCTAssertThrowsError(try QXK24Engine.shared.cycle())
    }
    func test_healthScore_nonNegative() throws {
        try QXK24Engine.shared.start()
        XCTAssertGreaterThanOrEqual(QXK24Engine.shared.healthScore, 0.0)
    }
    func test_healthScore_notAbove100() throws {
        try QXK24Engine.shared.start()
        XCTAssertLessThanOrEqual(QXK24Engine.shared.healthScore, 100.0)
    }
    func test_healthSnapshot_notNil() throws {
        try QXK24Engine.shared.start()
        XCTAssertNotNil(QXK24Engine.shared.healthSnapshot)
    }
    func test_healthSnapshot_cycleMatches() throws {
        try QXK24Engine.shared.start()
        XCTAssertEqual(QXK24Engine.shared.healthSnapshot?.cycleIndex, 0)
    }
    func test_registerAllGovernors_succeeds() throws {
        try QXK24Engine.shared.start()
        XCTAssertNoThrow(try QXK24Engine.shared.registerAllGovernors())
    }
    func test_activeResources_sixAfterRegistration() throws {
        try QXK24Engine.shared.start()
        try QXK24Engine.shared.registerAllGovernors()
        XCTAssertEqual(QXK24Engine.shared.activeResources, 6)
    }
    func test_generateReport_succeeds() throws {
        try QXK24Engine.shared.start()
        try QXK24Engine.shared.registerAllGovernors()
        try QXK24Engine.shared.cycle()
        XCTAssertNoThrow(try QXK24Engine.shared.generateReport())
    }
    func test_generateReport_constitutionallySound() throws {
        try QXK24Engine.shared.start()
        try QXK24Engine.shared.registerAllGovernors()
        try QXK24Engine.shared.cycle()
        XCTAssertTrue(try QXK24Engine.shared.generateReport().constitutionallySound)
    }
    func test_generateReport_zeroViolations() throws {
        try QXK24Engine.shared.start()
        try QXK24Engine.shared.registerAllGovernors()
        try QXK24Engine.shared.cycle()
        XCTAssertEqual(try QXK24Engine.shared.generateReport().violationCount, 0)
    }
    func test_generateReport_professionalOrBetter() throws {
        try QXK24Engine.shared.start()
        try QXK24Engine.shared.registerAllGovernors()
        try QXK24Engine.shared.cycle()
        XCTAssertGreaterThanOrEqual(try QXK24Engine.shared.generateReport().healthScore, 80.0)
    }
    func test_onHealthChanged_calledAfterCycle() throws {
        try QXK24Engine.shared.start()
        var called = false
        QXK24Engine.shared.onHealthChanged { _ in called = true }
        try QXK24Engine.shared.cycle()
        XCTAssertTrue(called)
    }
}
