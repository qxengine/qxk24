/* ============================================================
 * QXK24Test.kt
 * QXK24 Universal Kernel - Android Kotlin Bridge Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/platform/android/QXK24Test.kt
 * License : Apache 2.0
 *
 * Description:
 * Provides 30 Phase 4 proofs for the Kotlin platform bridge.
 * ============================================================ */

package com.qxengine.qxk24

import org.junit.After
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertThrows
import org.junit.Assert.assertTrue
import org.junit.Before
import org.junit.Test

class QXK24EngineTest {
    @Before
    fun setUp() {
        QXK24Engine.start()
    }

    @After
    fun tearDown() {
        QXK24Engine.stop()
    }

    @Test fun version_isAlpha4() {
        assertEquals("1.0.0-alpha.4", QXK24Engine.version)
    }
    @Test fun founder_isMasaBayu() {
        assertEquals("Masa Bayu", QXK24Engine.founder)
    }
    @Test fun science_containsAlamtologi() {
        assertTrue(QXK24Engine.science.contains("ALAMTOLOGI"))
    }
    @Test fun certTier_sovereign_threshold() {
        assertEquals(95.0f, QXK24CertTier.SOVEREIGN.threshold)
    }
    @Test fun certTier_professional_threshold() {
        assertEquals(80.0f, QXK24CertTier.PROFESSIONAL.threshold)
    }
    @Test fun certTier_standard_label() {
        assertEquals("STANDARD", QXK24CertTier.STANDARD.label)
    }
    @Test fun certTier_fromValue_unknown_isUncertified() {
        assertEquals(QXK24CertTier.UNCERTIFIED, QXK24CertTier.fromValue(99))
    }
    @Test fun direction_as_role() {
        assertEquals("Governance", QXK24Direction.AS.role)
    }
    @Test fun direction_bh_role() {
        assertEquals("Hardware", QXK24Direction.BH.role)
    }
    @Test fun direction_as_pair_is_bh() {
        assertEquals(QXK24Direction.BH, QXK24Direction.AS.pair)
    }
    @Test fun direction_kn_pair_is_kr() {
        assertEquals(QXK24Direction.KR, QXK24Direction.KN.pair)
    }
    @Test fun direction_dn_pair_is_bg() {
        assertEquals(QXK24Direction.BG, QXK24Direction.DN.pair)
    }
    @Test fun isRunning_afterStart() {
        assertTrue(QXK24Engine.isRunning)
    }
    @Test fun isNotRunning_afterStop() {
        QXK24Engine.stop()
        assertFalse(QXK24Engine.isRunning)
    }
    @Test fun activeResources_startsAtZero() {
        assertEquals(0, QXK24Engine.activeResources)
    }
    @Test fun cycleIndex_startsAtZero() {
        assertEquals(0L, QXK24Engine.cycleIndex)
    }
    @Test fun cycleIndex_incrementsAfterCycle() {
        QXK24Engine.cycle()
        assertEquals(1L, QXK24Engine.cycleIndex)
    }
    @Test fun cycle_throwsWhenStopped() {
        QXK24Engine.stop()
        assertThrows(QXK24Exception.NotRunning::class.java) {
            QXK24Engine.cycle()
        }
    }
    @Test fun healthScore_nonNegative() {
        assertTrue(QXK24Engine.healthScore >= 0.0f)
    }
    @Test fun healthScore_notAbove100() {
        assertTrue(QXK24Engine.healthScore <= 100.0f)
    }
    @Test fun healthSnapshot_cycleIndexMatches() {
        assertEquals(QXK24Engine.cycleIndex, QXK24Engine.healthSnapshot.cycleIndex)
    }
    @Test fun healthSnapshot_initialIsCertified() {
        assertTrue(QXK24Engine.healthSnapshot.isCertified)
    }
    @Test fun registerAllGovernors_succeeds() {
        QXK24Engine.registerAllGovernors()
        assertEquals(6, QXK24Engine.activeResources)
    }
    @Test fun activeResources_sixAfterRegistration() {
        QXK24Engine.registerAllGovernors()
        assertEquals(6, QXK24Engine.activeResources)
    }
    @Test fun generateReport_succeeds() {
        QXK24Engine.registerAllGovernors()
        QXK24Engine.cycle()
        assertTrue(QXK24Engine.generateReport().healthScore >= 0.0f)
    }
    @Test fun generateReport_constitutionallySound() {
        QXK24Engine.registerAllGovernors()
        QXK24Engine.cycle()
        assertTrue(QXK24Engine.generateReport().constitutionallySound)
    }
    @Test fun generateReport_zeroViolations() {
        QXK24Engine.registerAllGovernors()
        QXK24Engine.cycle()
        assertEquals(0, QXK24Engine.generateReport().violationCount)
    }
    @Test fun generateReport_activeResourcesSix() {
        QXK24Engine.registerAllGovernors()
        QXK24Engine.cycle()
        assertEquals(6, QXK24Engine.generateReport().activeResources)
    }
    @Test fun generateReport_professionalOrBetter() {
        QXK24Engine.registerAllGovernors()
        QXK24Engine.cycle()
        assertTrue(QXK24Engine.generateReport().healthScore >= 80.0f)
    }
    @Test fun onHealthChanged_calledAfterCycle() {
        var called = false
        QXK24Engine.onHealthChanged = { called = true }
        QXK24Engine.cycle()
        assertTrue(called)
    }
}
