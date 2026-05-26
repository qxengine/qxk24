/* ============================================================
 * QXK24.swift
 * QXK24 Universal Kernel - Swift Public API Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : platform/ios/QXK24.swift
 * License : Apache 2.0
 *
 * Description:
 * Exposes a small Swift API over the Objective-C QXK24 bridge.
 * ============================================================ */

import Foundation

public let QXK24_VERSION = "1.0.0-alpha.4"
public let QXK24_FOUNDER = "Masa Bayu"
public let QXK24_SCIENCE = "ALAMTOLOGI - Quranic Science"

public enum QXK24CertTier: Int {
    case uncertified = 0
    case standard = 1
    case professional = 2
    case sovereign = 3

    public var label: String {
        switch self {
        case .uncertified: return "UNCERTIFIED"
        case .standard: return "STANDARD"
        case .professional: return "PROFESSIONAL"
        case .sovereign: return "SOVEREIGN"
        }
    }

    public var threshold: Float {
        switch self {
        case .uncertified: return 0.0
        case .standard: return 60.0
        case .professional: return 80.0
        case .sovereign: return 95.0
        }
    }
}

public enum QXK24Direction: Int {
    case as_ = 0
    case bh = 1
    case dn = 2
    case bg = 3
    case kn = 4
    case kr = 5

    public var role: String {
        switch self {
        case .as_: return "Governance"
        case .bh: return "Hardware"
        case .dn: return "Execution"
        case .bg: return "Storage"
        case .kn: return "Output"
        case .kr: return "Input"
        }
    }

    public var pair: QXK24Direction {
        switch self {
        case .as_: return .bh
        case .bh: return .as_
        case .dn: return .bg
        case .bg: return .dn
        case .kn: return .kr
        case .kr: return .kn
        }
    }
}

public enum QXK24Error: Error {
    case notRunning
    case registrationFailed
    case reportFailed
    case nullKernel
}

public struct QXK24HealthSnapshot {
    public let score: Float
    public let tier: QXK24CertTier
    public let cycleIndex: UInt64
    public let timestampMs: UInt64

    public var isCertified: Bool {
        tier != .uncertified
    }
}

public struct QXK24Report {
    public let cycleIndex: UInt64
    public let healthScore: Float
    public let tier: QXK24CertTier
    public let violationCount: UInt32
    public let constitutionallySound: Bool
    public let activeResources: UInt32
}

public final class QXK24Engine {
    public static let shared = QXK24Engine()

    private let bridge = QXK24Bridge.shared()
    private var healthObserver: ((QXK24HealthSnapshot) -> Void)?

    private init() {}

    public var isRunning: Bool {
        bridge.isRunning()
    }

    public var version: String { QXK24_VERSION }
    public var founder: String { QXK24_FOUNDER }
    public var science: String { QXK24_SCIENCE }

    @discardableResult
    public func start() throws -> Bool {
        guard bridge.start() else {
            throw QXK24Error.nullKernel
        }
        return true
    }

    public func stop() {
        bridge.stop()
    }

    public func cycle() throws {
        guard isRunning else { throw QXK24Error.notRunning }
        bridge.cycle()
        notifyHealthObserver()
    }

    public var cycleIndex: UInt64 {
        bridge.cycleIndex()
    }

    public var healthScore: Float {
        bridge.healthScore()
    }

    public var certTier: QXK24CertTier {
        QXK24CertTier(rawValue: bridge.certTier().rawValue) ?? .uncertified
    }

    public var healthSnapshot: QXK24HealthSnapshot? {
        guard let snapshot = bridge.healthSnapshot() else { return nil }
        return QXK24HealthSnapshot(
            score: snapshot.score,
            tier: QXK24CertTier(rawValue: snapshot.tier.rawValue) ?? .uncertified,
            cycleIndex: snapshot.cycleIndex,
            timestampMs: snapshot.timestampMs
        )
    }

    public func registerAllGovernors(
        memoryBytes: UInt64 = 256 * 1024 * 1024,
        cpuPercent: UInt32 = 100,
        networkBandwidth: UInt64 = 100 * 1024 * 1024,
        storageBytes: UInt64 = 512 * 1024 * 1024,
        ioChannels: UInt32 = 64,
        energyPercent: UInt32 = 100
    ) throws {
        guard isRunning else { throw QXK24Error.notRunning }
        let ok = bridge.registerMemoryGovernor(withBytes: memoryBytes) &&
            bridge.registerCPUGovernor(withPercent: cpuPercent) &&
            bridge.registerNetworkGovernor(withBandwidth: networkBandwidth) &&
            bridge.registerStorageGovernor(withBytes: storageBytes) &&
            bridge.registerIOGovernor(withChannels: ioChannels) &&
            bridge.registerEnergyGovernor(withPercent: energyPercent)
        guard ok else { throw QXK24Error.registrationFailed }
    }

    public var activeResources: UInt32 {
        bridge.activeResources()
    }

    public func generateReport() throws -> QXK24Report {
        guard let raw = bridge.generateReport() else {
            throw QXK24Error.reportFailed
        }
        return QXK24Report(
            cycleIndex: raw.cycleIndex,
            healthScore: raw.healthScore,
            tier: QXK24CertTier(rawValue: raw.tier.rawValue) ?? .uncertified,
            violationCount: raw.violationCount,
            constitutionallySound: raw.constitutionallySound,
            activeResources: raw.activeResources
        )
    }

    public func onHealthChanged(_ callback: @escaping (QXK24HealthSnapshot) -> Void) {
        healthObserver = callback
    }

    internal func notifyHealthObserver() {
        guard let snapshot = healthSnapshot else { return }
        healthObserver?(snapshot)
    }
}
