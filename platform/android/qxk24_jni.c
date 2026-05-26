/* ============================================================
 * qxk24_jni.c
 * QXK24 Universal Kernel - Android JNI Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : platform/android/qxk24_jni.c
 * License : Apache 2.0
 *
 * Description:
 * Bridges Kotlin calls to the Phase 3 C kernel ABI.
 * ============================================================ */

#include "qxk24/kernel/qxk24_report.h"
#include <jni.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static qxk24_kernel_t s_kernel = NULL;
static bool s_running = false;

static uint64_t canonical_budget(uint64_t requested) {
    const uint64_t budget =
        qxk24_ama_core_value((uint32_t)QXK24_LEVEL_SISTEM + 1U) *
        QXK24_BASE_UNIT_BYTES;
    return requested > budget ? requested : budget;
}

static bool ensure_kernel(void) {
    if (s_kernel != NULL) return true;
    QXK24Config cfg = qxk24_default_config();
    return qxk24_create(&cfg, &s_kernel) == QXK24_OK;
}

static jboolean register_resource(QXK24ResourceType type,
                                  QXK24Direction direction,
                                  uint64_t requested_budget) {
    if (s_kernel == NULL) return JNI_FALSE;

    uint32_t id = 0U;
    const uint64_t budget = canonical_budget(requested_budget);
    if (qxk24_resource_register(s_kernel, type, direction,
            QXK24_LEVEL_SISTEM, budget, &id) != QXK24_OK) {
        return JNI_FALSE;
    }
    return qxk24_resource_update(s_kernel, id, budget / 2ULL, 1.0) ==
        QXK24_OK ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeStart(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    if (!ensure_kernel()) return JNI_FALSE;
    if (s_running) return JNI_TRUE;
    s_running = qxk24_start(s_kernel) == QXK24_OK;
    return s_running ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeStop(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    if (s_kernel == NULL) return;
    if (s_running) qxk24_stop(s_kernel);
    qxk24_destroy(s_kernel);
    s_kernel = NULL;
    s_running = false;
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeIsRunning(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    return s_running ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeCycle(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    if (s_kernel != NULL && s_running) (void)qxk24_cycle(s_kernel, NULL);
}

JNIEXPORT jlong JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeCycleIndex(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    return s_kernel == NULL ? 0L : (jlong)qxk24_cycle_index(s_kernel);
}

JNIEXPORT jfloat JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeHealthScore(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    return s_kernel == NULL ? 0.0f : (jfloat)qxk24_health_score(s_kernel);
}

JNIEXPORT jint JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeCertTier(JNIEnv *env, jobject obj) {
    (void)env;
    (void)obj;
    return s_kernel == NULL ? 0 : (jint)qxk24_cert_tier(s_kernel);
}

JNIEXPORT jint JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeActiveResources(JNIEnv *env,
                                                       jobject obj) {
    (void)env;
    (void)obj;
    return s_kernel == NULL ? 0 : (jint)qxk24_active_resources(s_kernel);
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeRegisterMemory(JNIEnv *env,
                                                      jobject obj,
                                                      jlong bytes) {
    (void)env;
    (void)obj;
    return register_resource(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS,
        (uint64_t)bytes);
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeRegisterCPU(JNIEnv *env,
                                                   jobject obj,
                                                   jint percent) {
    (void)env;
    (void)obj;
    return register_resource(QXK24_RESOURCE_CPU, QXK24_DIR_DN,
        (uint64_t)percent);
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeRegisterNetwork(JNIEnv *env,
                                                       jobject obj,
                                                       jlong bandwidth) {
    (void)env;
    (void)obj;
    return register_resource(QXK24_RESOURCE_NETWORK, QXK24_DIR_KN,
        (uint64_t)bandwidth);
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeRegisterStorage(JNIEnv *env,
                                                       jobject obj,
                                                       jlong bytes) {
    (void)env;
    (void)obj;
    return register_resource(QXK24_RESOURCE_STORAGE, QXK24_DIR_BG,
        (uint64_t)bytes);
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeRegisterIO(JNIEnv *env,
                                                  jobject obj,
                                                  jint channels) {
    (void)env;
    (void)obj;
    return register_resource(QXK24_RESOURCE_IO, QXK24_DIR_BH,
        (uint64_t)channels);
}

JNIEXPORT jboolean JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeRegisterEnergy(JNIEnv *env,
                                                      jobject obj,
                                                      jint percent) {
    (void)env;
    (void)obj;
    return register_resource(QXK24_RESOURCE_ENERGY, QXK24_DIR_KR,
        (uint64_t)percent);
}

JNIEXPORT jfloatArray JNICALL
Java_com_qxengine_qxk24_QXK24JNI_nativeGenerateReport(JNIEnv *env,
                                                      jobject obj) {
    (void)obj;
    if (s_kernel == NULL) return NULL;

    QXK24Report raw;
    memset(&raw, 0, sizeof(raw));
    if (qxk24_report_generate(s_kernel, &raw) != QXK24_OK) return NULL;

    jfloat data[6] = {
        (jfloat)raw.health_score,
        (jfloat)raw.cert_tier,
        raw.health_score >= QXK24_HEALTH_STANDARD ? 0.0f : 1.0f,
        qxk24_report_is_sound(&raw) ? 1.0f : 0.0f,
        (jfloat)raw.active_resources,
        (jfloat)raw.cycle_index
    };
    jfloatArray arr = (*env)->NewFloatArray(env, 6);
    if (arr == NULL) return NULL;
    (*env)->SetFloatArrayRegion(env, arr, 0, 6, data);
    return arr;
}
