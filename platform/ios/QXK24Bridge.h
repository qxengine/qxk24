/* ============================================================
 * QXK24Bridge.h
 * QXK24 Universal Kernel - Objective-C Swift Shim
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : platform/ios/QXK24Bridge.h
 * License : Apache 2.0
 *
 * Description:
 * Bridges the QXK24 C kernel to the Swift public API.
 * ============================================================ */

#ifndef QXK24Bridge_h
#define QXK24Bridge_h

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, QXK24BridgeCertTier) {
    QXK24BridgeCertUncertified = 0,
    QXK24BridgeCertStandard = 1,
    QXK24BridgeCertProfessional = 2,
    QXK24BridgeCertSovereign = 3
};

typedef NS_ENUM(NSInteger, QXK24BridgeDirection) {
    QXK24BridgeDirAs = 0,
    QXK24BridgeDirBh = 1,
    QXK24BridgeDirDn = 2,
    QXK24BridgeDirBg = 3,
    QXK24BridgeDirKn = 4,
    QXK24BridgeDirKr = 5
};

@interface QXK24BridgeHealthSnapshot : NSObject
@property (nonatomic, assign) float score;
@property (nonatomic, assign) QXK24BridgeCertTier tier;
@property (nonatomic, assign) uint64_t cycleIndex;
@property (nonatomic, assign) uint64_t timestampMs;
@end

@interface QXK24BridgeReport : NSObject
@property (nonatomic, assign) uint64_t cycleIndex;
@property (nonatomic, assign) float healthScore;
@property (nonatomic, assign) QXK24BridgeCertTier tier;
@property (nonatomic, assign) uint32_t violationCount;
@property (nonatomic, assign) BOOL constitutionallySound;
@property (nonatomic, assign) uint32_t activeResources;
@end

@interface QXK24Bridge : NSObject

+ (instancetype)shared;

- (BOOL)start;
- (void)stop;
- (BOOL)isRunning;

- (void)cycle;
- (uint64_t)cycleIndex;

- (float)healthScore;
- (QXK24BridgeCertTier)certTier;
- (nullable QXK24BridgeHealthSnapshot *)healthSnapshot;

- (BOOL)registerMemoryGovernorWithBytes:(uint64_t)bytes;
- (BOOL)registerCPUGovernorWithPercent:(uint32_t)percent;
- (BOOL)registerNetworkGovernorWithBandwidth:(uint64_t)bandwidth;
- (BOOL)registerStorageGovernorWithBytes:(uint64_t)bytes;
- (BOOL)registerIOGovernorWithChannels:(uint32_t)channels;
- (BOOL)registerEnergyGovernorWithPercent:(uint32_t)percent;
- (uint32_t)activeResources;

- (nullable QXK24BridgeReport *)generateReport;

@end

NS_ASSUME_NONNULL_END

#endif /* QXK24Bridge_h */
