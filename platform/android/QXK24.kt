/* ============================================================
 * QXK24.kt
 * QXK24 Universal Kernel - Kotlin Public API Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : platform/android/QXK24.kt
 * License : Apache 2.0
 *
 * Description:
 * Exposes a compact Kotlin API over the JNI bridge.
 * ============================================================ */

package com.qxengine.qxk24

const val QXK24_VERSION = "1.0.0-alpha.4"
const val QXK24_FOUNDER = "Masa Bayu"
const val QXK24_SCIENCE = "ALAMTOLOGI - Quranic Science"

enum class QXK24CertTier(
    val value: Int,
    val label: String,
    val threshold: Float
) {
    UNCERTIFIED(0, "UNCERTIFIED", 0.0f),
    STANDARD(1, "STANDARD", 60.0f),
    PROFESSIONAL(2, "PROFESSIONAL", 80.0f),
    SOVEREIGN(3, "SOVEREIGN", 95.0f);

    companion object {
        fun fromValue(value: Int): QXK24CertTier =
            values().firstOrNull { it.value == value } ?: UNCERTIFIED
    }
}

enum class QXK24Direction(val value: Int, val role: String) {
    AS(0, "Governance"),
    BH(1, "Hardware"),
    DN(2, "Execution"),
    BG(3, "Storage"),
    KN(4, "Output"),
    KR(5, "Input");

    val pair: QXK24Direction
        get() = when (this) {
            AS -> BH
            BH -> AS
            DN -> BG
            BG -> DN
            KN -> KR
            KR -> KN
        }
}

sealed class QXK24Exception(message: String) : Exception(message) {
    class NotRunning : QXK24Exception("Kernel is not running")
    class RegistrationFailed : QXK24Exception("Governor registration failed")
    class ReportFailed : QXK24Exception("Report generation failed")
    class NullKernel : QXK24Exception("Kernel handle is null")
}

data class QXK24HealthSnapshot(
    val score: Float,
    val tier: QXK24CertTier,
    val cycleIndex: Long,
    val timestampMs: Long
) {
    val isCertified: Boolean get() = tier != QXK24CertTier.UNCERTIFIED
}

data class QXK24Report(
    val cycleIndex: Long,
    val healthScore: Float,
    val tier: QXK24CertTier,
    val violationCount: Int,
    val constitutionallySound: Boolean,
    val activeResources: Int
)

private object QXK24JNI {
    init {
        System.loadLibrary("qxk24")
    }

    external fun nativeStart(): Boolean
    external fun nativeStop()
    external fun nativeIsRunning(): Boolean
    external fun nativeCycle()
    external fun nativeCycleIndex(): Long
    external fun nativeHealthScore(): Float
    external fun nativeCertTier(): Int
    external fun nativeActiveResources(): Int

    external fun nativeRegisterMemory(bytes: Long): Boolean
    external fun nativeRegisterCPU(percent: Int): Boolean
    external fun nativeRegisterNetwork(bandwidth: Long): Boolean
    external fun nativeRegisterStorage(bytes: Long): Boolean
    external fun nativeRegisterIO(channels: Int): Boolean
    external fun nativeRegisterEnergy(percent: Int): Boolean

    external fun nativeGenerateReport(): FloatArray?
}

object QXK24Engine {
    val isRunning: Boolean get() = QXK24JNI.nativeIsRunning()
    val cycleIndex: Long get() = QXK24JNI.nativeCycleIndex()
    val healthScore: Float get() = QXK24JNI.nativeHealthScore()
    val activeResources: Int get() = QXK24JNI.nativeActiveResources()
    val version: String get() = QXK24_VERSION
    val founder: String get() = QXK24_FOUNDER
    val science: String get() = QXK24_SCIENCE

    val certTier: QXK24CertTier
        get() = QXK24CertTier.fromValue(QXK24JNI.nativeCertTier())

    val healthSnapshot: QXK24HealthSnapshot
        get() = QXK24HealthSnapshot(
            score = healthScore,
            tier = certTier,
            cycleIndex = cycleIndex,
            timestampMs = System.currentTimeMillis()
        )

    var onHealthChanged: ((QXK24HealthSnapshot) -> Unit)? = null

    @Throws(QXK24Exception.NullKernel::class)
    fun start(): Boolean {
        if (!QXK24JNI.nativeStart()) throw QXK24Exception.NullKernel()
        return true
    }

    fun stop() {
        QXK24JNI.nativeStop()
    }

    @Throws(QXK24Exception.NotRunning::class)
    fun cycle() {
        if (!isRunning) throw QXK24Exception.NotRunning()
        QXK24JNI.nativeCycle()
        onHealthChanged?.invoke(healthSnapshot)
    }

    @Throws(
        QXK24Exception.NotRunning::class,
        QXK24Exception.RegistrationFailed::class
    )
    fun registerAllGovernors(
        memoryBytes: Long = 256L * 1024L * 1024L,
        cpuPercent: Int = 100,
        networkBandwidth: Long = 100L * 1024L * 1024L,
        storageBytes: Long = 512L * 1024L * 1024L,
        ioChannels: Int = 64,
        energyPercent: Int = 100
    ) {
        if (!isRunning) throw QXK24Exception.NotRunning()
        val ok = QXK24JNI.nativeRegisterMemory(memoryBytes) &&
            QXK24JNI.nativeRegisterCPU(cpuPercent) &&
            QXK24JNI.nativeRegisterNetwork(networkBandwidth) &&
            QXK24JNI.nativeRegisterStorage(storageBytes) &&
            QXK24JNI.nativeRegisterIO(ioChannels) &&
            QXK24JNI.nativeRegisterEnergy(energyPercent)
        if (!ok) throw QXK24Exception.RegistrationFailed()
    }

    @Throws(QXK24Exception.ReportFailed::class)
    fun generateReport(): QXK24Report {
        val data = QXK24JNI.nativeGenerateReport()
            ?: throw QXK24Exception.ReportFailed()
        return QXK24Report(
            cycleIndex = data[5].toLong(),
            healthScore = data[0],
            tier = QXK24CertTier.fromValue(data[1].toInt()),
            violationCount = data[2].toInt(),
            constitutionallySound = data[3] == 1.0f,
            activeResources = data[4].toInt()
        )
    }
}
