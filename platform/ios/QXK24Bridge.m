/* ============================================================
 * QXK24Bridge.m
 * QXK24 Universal Kernel - Objective-C Swift Shim Implementation
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : platform/ios/QXK24Bridge.m
 * License : Apache 2.0
 *
 * Description:
 * Implements the Objective-C bridge on top of the Phase 3 C kernel ABI.
 * ============================================================ */

#import "QXK24Bridge.h"
#include "qxk24/kernel/qxk24_report.h"
#include <dispatch/dispatch.h>
#include <string.h>

@implementation QXK24BridgeHealthSnapshot
@end

@implementation QXK24BridgeReport
@end

@implementation QXK24Bridge {
    qxk24_kernel_t _kernel;
    BOOL _running;
}

+ (instancetype)shared {
    static QXK24Bridge *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[QXK24Bridge alloc] init];
    });
    return instance;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        QXK24Config cfg = qxk24_default_config();
        _kernel = NULL;
        _running = NO;
        (void)qxk24_create(&cfg, &_kernel);
    }
    return self;
}

- (void)dealloc {
    if (_kernel != NULL) {
        if (_running) qxk24_stop(_kernel);
        qxk24_destroy(_kernel);
        _kernel = NULL;
    }
}

- (BOOL)ensureKernel {
    if (_kernel != NULL) return YES;
    QXK24Config cfg = qxk24_default_config();
    return qxk24_create(&cfg, &_kernel) == QXK24_OK;
}

- (BOOL)start {
    if (![self ensureKernel]) return NO;
    if (_running) return YES;
    _running = (qxk24_start(_kernel) == QXK24_OK);
    return _running;
}

- (void)stop {
    if (_kernel == NULL) {
        _running = NO;
        return;
    }
    if (_running) qxk24_stop(_kernel);
    qxk24_destroy(_kernel);
    _kernel = NULL;
    _running = NO;
}

- (BOOL)isRunning {
    return _running;
}

- (void)cycle {
    if (_kernel == NULL || !_running) return;
    (void)qxk24_cycle(_kernel, NULL);
}

- (uint64_t)cycleIndex {
    return _kernel == NULL ? 0ULL : qxk24_cycle_index(_kernel);
}

- (float)healthScore {
    return _kernel == NULL ? 0.0f : (float)qxk24_health_score(_kernel);
}

- (QXK24BridgeCertTier)certTier {
    return _kernel == NULL
        ? QXK24BridgeCertUncertified
        : (QXK24BridgeCertTier)qxk24_cert_tier(_kernel);
}

- (nullable QXK24BridgeHealthSnapshot *)healthSnapshot {
    if (_kernel == NULL) return nil;
    QXK24BridgeHealthSnapshot *snapshot =
        [[QXK24BridgeHealthSnapshot alloc] init];
    snapshot.score = (float)qxk24_health_score(_kernel);
    snapshot.tier = (QXK24BridgeCertTier)qxk24_cert_tier(_kernel);
    snapshot.cycleIndex = qxk24_cycle_index(_kernel);
    snapshot.timestampMs =
        (uint64_t)([[NSDate date] timeIntervalSince1970] * 1000.0);
    return snapshot;
}

static uint64_t QXK24BridgeBudget(uint64_t requested) {
    const uint64_t canonical =
        qxk24_ama_core_value((uint32_t)QXK24_LEVEL_SISTEM + 1U) *
        QXK24_BASE_UNIT_BYTES;
    return requested > canonical ? requested : canonical;
}

- (BOOL)registerResourceType:(QXK24ResourceType)type
                   direction:(QXK24Direction)direction
                      budget:(uint64_t)budget {
    if (_kernel == NULL) return NO;
    uint32_t resourceId = 0U;
    const uint64_t soft = QXK24BridgeBudget(budget);
    qxk24_err_t err = qxk24_resource_register(_kernel, type, direction,
        QXK24_LEVEL_SISTEM, soft, &resourceId);
    if (err != QXK24_OK) return NO;
    return qxk24_resource_update(_kernel, resourceId, soft / 2ULL, 1.0) ==
        QXK24_OK;
}

- (BOOL)registerMemoryGovernorWithBytes:(uint64_t)bytes {
    return [self registerResourceType:QXK24_RESOURCE_MEMORY
                            direction:QXK24_DIR_AS
                               budget:bytes];
}

- (BOOL)registerCPUGovernorWithPercent:(uint32_t)percent {
    return [self registerResourceType:QXK24_RESOURCE_CPU
                            direction:QXK24_DIR_DN
                               budget:(uint64_t)percent];
}

- (BOOL)registerNetworkGovernorWithBandwidth:(uint64_t)bandwidth {
    return [self registerResourceType:QXK24_RESOURCE_NETWORK
                            direction:QXK24_DIR_KN
                               budget:bandwidth];
}

- (BOOL)registerStorageGovernorWithBytes:(uint64_t)bytes {
    return [self registerResourceType:QXK24_RESOURCE_STORAGE
                            direction:QXK24_DIR_BG
                               budget:bytes];
}

- (BOOL)registerIOGovernorWithChannels:(uint32_t)channels {
    return [self registerResourceType:QXK24_RESOURCE_IO
                            direction:QXK24_DIR_BH
                               budget:(uint64_t)channels];
}

- (BOOL)registerEnergyGovernorWithPercent:(uint32_t)percent {
    return [self registerResourceType:QXK24_RESOURCE_ENERGY
                            direction:QXK24_DIR_KR
                               budget:(uint64_t)percent];
}

- (uint32_t)activeResources {
    return _kernel == NULL ? 0U : qxk24_active_resources(_kernel);
}

- (nullable QXK24BridgeReport *)generateReport {
    if (_kernel == NULL) return nil;
    QXK24Report raw;
    memset(&raw, 0, sizeof(raw));
    if (qxk24_report_generate(_kernel, &raw) != QXK24_OK) return nil;

    QXK24BridgeReport *report = [[QXK24BridgeReport alloc] init];
    report.cycleIndex = raw.cycle_index;
    report.healthScore = (float)raw.health_score;
    report.tier = (QXK24BridgeCertTier)raw.cert_tier;
    report.violationCount = raw.health_score >= QXK24_HEALTH_STANDARD ? 0U : 1U;
    report.constitutionallySound = qxk24_report_is_sound(&raw);
    report.activeResources = raw.active_resources;
    return report;
}

@end
