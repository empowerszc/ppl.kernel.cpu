#include <immintrin.h>

#include "ppl/kernel/x86/fp32/conv2d/direct/fma/conv2d_n16cx_direct_kernel_fp32_fma.h"

namespace ppl { namespace kernel { namespace x86 {

template <int64_t stride_w, bool nt_store, bool prefetch_src, int64_t oc_len, int64_t w_len>
void conv2d_n16cx_direct_fp32_fma_blk1x6_kernel(
    const int64_t *priv_param,
    const int64_t *shar_param)
{
#define IC_COMPUTE_STEP(IC) do {\
    if (oc_len > 0 * OC_RF_BLK()) ymm6 = _mm256_loadu_ps(ic_flt + (IC) * CH_DT_BLK() + 0 * OC_RF_BLK());\
    if (oc_len > 1 * OC_RF_BLK()) ymm7 = _mm256_loadu_ps(ic_flt + (IC) * CH_DT_BLK() + 1 * OC_RF_BLK());\
    if (w_len > 0) {\
        ymm8 = _mm256_set1_ps(ic_src[(IC) + 0 * stride_w * CH_DT_BLK()]);\
        if (oc_len > 0 * OC_RF_BLK()) ymm0 = _mm256_fmadd_ps(ymm6, ymm8, ymm0);\
        if (oc_len > 1 * OC_RF_BLK()) ymm10 = _mm256_fmadd_ps(ymm7, ymm8, ymm10);\
    }\
    if (w_len > 1) {\
        ymm9 = _mm256_set1_ps(ic_src[(IC) + 1 * stride_w * CH_DT_BLK()]);\
        if (oc_len > 0 * OC_RF_BLK()) ymm1 = _mm256_fmadd_ps(ymm6, ymm9, ymm1);\
        if (oc_len > 1 * OC_RF_BLK()) ymm11 = _mm256_fmadd_ps(ymm7, ymm9, ymm11);\
    }\
    if (w_len > 2) {\
        ymm8 = _mm256_set1_ps(ic_src[(IC) + 2 * stride_w * CH_DT_BLK()]);\
        if (oc_len > 0 * OC_RF_BLK()) ymm2 = _mm256_fmadd_ps(ymm6, ymm8, ymm2);\
        if (oc_len > 1 * OC_RF_BLK()) ymm12 = _mm256_fmadd_ps(ymm7, ymm8, ymm12);\
    }\
    if (w_len > 3) {\
        ymm9 = _mm256_set1_ps(ic_src[(IC) + 3 * stride_w * CH_DT_BLK()]);\
        if (oc_len > 0 * OC_RF_BLK()) ymm3 = _mm256_fmadd_ps(ymm6, ymm9, ymm3);\
        if (oc_len > 1 * OC_RF_BLK()) ymm13 = _mm256_fmadd_ps(ymm7, ymm9, ymm13);\
    }\
    if (w_len > 4) {\
        ymm8 = _mm256_set1_ps(ic_src[(IC) + 4 * stride_w * CH_DT_BLK()]);\
        if (oc_len > 0 * OC_RF_BLK()) ymm4 = _mm256_fmadd_ps(ymm6, ymm8, ymm4);\
        if (oc_len > 1 * OC_RF_BLK()) ymm14 = _mm256_fmadd_ps(ymm7, ymm8, ymm14);\
    }\
    if (w_len > 5) {\
        ymm9 = _mm256_set1_ps(ic_src[(IC) + 5 * stride_w * CH_DT_BLK()]);\
        if (oc_len > 0 * OC_RF_BLK()) ymm5 = _mm256_fmadd_ps(ymm6, ymm9, ymm5);\
        if (oc_len > 1 * OC_RF_BLK()) ymm15 = _mm256_fmadd_ps(ymm7, ymm9, ymm15);\
    }\
} while (false)

// #define DO_PREFETCH_FLT
#define DO_PREFETCH_SRC
#ifdef DO_PREFETCH_FLT
#define IC_PREFETCH_STEP(IC) do {\
    if (w_len > 2) _mm_prefetch((const char*)(ic_flt + (IC) * CH_DT_BLK() + CH_DT_BLK() * CH_DT_BLK()), _MM_HINT_T0);\
} while (false)
#else
#define IC_PREFETCH_STEP(IC)
#endif

    const int64_t src_icb_stride = PICK_PARAM(const int64_t, shar_param, SRC_ICB_STRIDE_IDX());
    const int64_t src_sh_stride = PICK_PARAM(const int64_t, shar_param, SRC_SH_STRIDE_IDX());
    const int64_t src_dh_stride = PICK_PARAM(const int64_t, shar_param, SRC_DH_STRIDE_IDX());
    const int64_t src_dw_stride = PICK_PARAM(const int64_t, shar_param, SRC_DW_STRIDE_IDX());

    const float *h_src = PICK_PARAM(const float*, priv_param, SRC_IDX());
    const float *h_his = PICK_PARAM(const float*, priv_param, HIS_IDX());
    float *h_dst = PICK_PARAM(float*, priv_param, DST_IDX());
    int64_t oh = PICK_PARAM(int64_t, priv_param, OH_IDX());
    do {
        const float *w_src = h_src;
        const float *w_his = h_his;
        float *w_dst = h_dst;
        int64_t ow = PICK_PARAM(int64_t, priv_param, OW_IDX());
        do {
            __m256 ymm0, ymm1, ymm2, ymm3, ymm4, ymm5, ymm6, ymm7;
            __m256 ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;
            { // session - initialize
                const uint64_t flags = PICK_PARAM(const uint64_t, shar_param, FLAGS_IDX());
                if (flags & KERNEL_FLAG_LD_BIAS()) {
                    const float *bias = PICK_PARAM(const float*, priv_param, BIAS_IDX());
                    if (oc_len > 0 * OC_RF_BLK()) {
                        if (w_len > 0) ymm0 = _mm256_loadu_ps(bias + 0 * OC_RF_BLK());
                        if (w_len > 1) ymm1 = ymm0;
                        if (w_len > 2) ymm2 = ymm0;
                        if (w_len > 3) ymm3 = ymm0;
                        if (w_len > 4) ymm4 = ymm0;
                        if (w_len > 5) ymm5 = ymm0;
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        if (w_len > 0) ymm10 = _mm256_loadu_ps(bias + 1 * OC_RF_BLK());
                        if (w_len > 1) ymm11 = ymm10;
                        if (w_len > 2) ymm12 = ymm10;
                        if (w_len > 3) ymm13 = ymm10;
                        if (w_len > 4) ymm14 = ymm10;
                        if (w_len > 5) ymm15 = ymm10;
                    }
                } else {
                    if (oc_len > 0 * OC_RF_BLK()) {
                        if (w_len > 0) ymm0 = _mm256_loadu_ps(w_his + 0 * CH_DT_BLK() + 0 * OC_RF_BLK());
                        if (w_len > 1) ymm1 = _mm256_loadu_ps(w_his + 1 * CH_DT_BLK() + 0 * OC_RF_BLK());
                        if (w_len > 2) ymm2 = _mm256_loadu_ps(w_his + 2 * CH_DT_BLK() + 0 * OC_RF_BLK());
                        if (w_len > 3) ymm3 = _mm256_loadu_ps(w_his + 3 * CH_DT_BLK() + 0 * OC_RF_BLK());
                        if (w_len > 4) ymm4 = _mm256_loadu_ps(w_his + 4 * CH_DT_BLK() + 0 * OC_RF_BLK());
                        if (w_len > 5) ymm5 = _mm256_loadu_ps(w_his + 5 * CH_DT_BLK() + 0 * OC_RF_BLK());
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        if (w_len > 0) ymm10 = _mm256_loadu_ps(w_his + 0 * CH_DT_BLK() + 1 * OC_RF_BLK());
                        if (w_len > 1) ymm11 = _mm256_loadu_ps(w_his + 1 * CH_DT_BLK() + 1 * OC_RF_BLK());
                        if (w_len > 2) ymm12 = _mm256_loadu_ps(w_his + 2 * CH_DT_BLK() + 1 * OC_RF_BLK());
                        if (w_len > 3) ymm13 = _mm256_loadu_ps(w_his + 3 * CH_DT_BLK() + 1 * OC_RF_BLK());
                        if (w_len > 4) ymm14 = _mm256_loadu_ps(w_his + 4 * CH_DT_BLK() + 1 * OC_RF_BLK());
                        if (w_len > 5) ymm15 = _mm256_loadu_ps(w_his + 5 * CH_DT_BLK() + 1 * OC_RF_BLK());
                    }
                }
                if (flags & KERNEL_FLAG_AD_BIAS()) {
                    const float *bias = PICK_PARAM(const float*, priv_param, BIAS_IDX());
                    if (oc_len > 0 * OC_RF_BLK()) {
                        ymm8 = _mm256_loadu_ps(bias + 0 * OC_RF_BLK());
                        if (w_len > 0) ymm0 = _mm256_add_ps(ymm8, ymm0);
                        if (w_len > 1) ymm1 = _mm256_add_ps(ymm8, ymm1);
                        if (w_len > 2) ymm2 = _mm256_add_ps(ymm8, ymm2);
                        if (w_len > 3) ymm3 = _mm256_add_ps(ymm8, ymm3);
                        if (w_len > 4) ymm4 = _mm256_add_ps(ymm8, ymm4);
                        if (w_len > 5) ymm5 = _mm256_add_ps(ymm8, ymm5);
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        ymm9 = _mm256_loadu_ps(bias + 1 * OC_RF_BLK());
                        if (w_len > 0) ymm10 = _mm256_add_ps(ymm9, ymm10);
                        if (w_len > 1) ymm11 = _mm256_add_ps(ymm9, ymm11);
                        if (w_len > 2) ymm12 = _mm256_add_ps(ymm9, ymm12);
                        if (w_len > 3) ymm13 = _mm256_add_ps(ymm9, ymm13);
                        if (w_len > 4) ymm14 = _mm256_add_ps(ymm9, ymm14);
                        if (w_len > 5) ymm15 = _mm256_add_ps(ymm9, ymm15);
                    }
                }
            }
            { // session - compute
                const float *k_flt = PICK_PARAM(const float*, priv_param, FLT_IDX());
                const float *icb_src = w_src;
                int64_t icb = PICK_PARAM(int64_t, shar_param, CHANNELS_IDX());
                do {
                    const float *k_src = icb_src;
                    int64_t kh = PICK_PARAM(int64_t, shar_param, KH_IDX());
                    do {
                        int64_t kw = PICK_PARAM(int64_t, shar_param, KW_IDX());
                        do {
                            if (icb >= CH_DT_BLK()) {
                                const float *ic_flt = k_flt;
                                const float *ic_src = k_src;
#ifdef DO_PREFETCH_SRC
                                if (prefetch_src) {
                                    if (oc_len > 1 * OC_RF_BLK()) { if (w_len > 2) {
                                        if (w_len > 0) _mm_prefetch((const char*)(ic_src + src_icb_stride + 0 * stride_w * CH_DT_BLK()), _MM_HINT_T0);
                                        if (w_len > 1) _mm_prefetch((const char*)(ic_src + src_icb_stride + 1 * stride_w * CH_DT_BLK()), _MM_HINT_T0);
                                        if (w_len > 2) _mm_prefetch((const char*)(ic_src + src_icb_stride + 2 * stride_w * CH_DT_BLK()), _MM_HINT_T0);
                                        if (w_len > 3) _mm_prefetch((const char*)(ic_src + src_icb_stride + 3 * stride_w * CH_DT_BLK()), _MM_HINT_T0);
                                        if (w_len > 4) _mm_prefetch((const char*)(ic_src + src_icb_stride + 4 * stride_w * CH_DT_BLK()), _MM_HINT_T0);
                                        if (w_len > 5) _mm_prefetch((const char*)(ic_src + src_icb_stride + 5 * stride_w * CH_DT_BLK()), _MM_HINT_T0);
                                    }}
                                }
#endif
                                IC_COMPUTE_STEP(0);
                                IC_PREFETCH_STEP(0);
                                IC_COMPUTE_STEP(1);
                                IC_PREFETCH_STEP(1);
                                IC_COMPUTE_STEP(2);
                                IC_PREFETCH_STEP(2);
                                IC_COMPUTE_STEP(3);
                                IC_PREFETCH_STEP(3);
                                IC_COMPUTE_STEP(4);
                                IC_PREFETCH_STEP(4);
                                IC_COMPUTE_STEP(5);
                                IC_PREFETCH_STEP(5);
                                IC_COMPUTE_STEP(6);
                                IC_PREFETCH_STEP(6);
                                IC_COMPUTE_STEP(7);
                                IC_PREFETCH_STEP(7);
                                IC_COMPUTE_STEP(8);
                                IC_PREFETCH_STEP(8);
                                IC_COMPUTE_STEP(9);
                                IC_PREFETCH_STEP(9);
                                IC_COMPUTE_STEP(10);
                                IC_PREFETCH_STEP(10);
                                IC_COMPUTE_STEP(11);
                                IC_PREFETCH_STEP(11);
                                IC_COMPUTE_STEP(12);
                                IC_PREFETCH_STEP(12);
                                IC_COMPUTE_STEP(13);
                                IC_PREFETCH_STEP(13);
                                IC_COMPUTE_STEP(14);
                                IC_PREFETCH_STEP(14);
                                IC_COMPUTE_STEP(15);
                                IC_PREFETCH_STEP(15);
                            } else {
                                const float *ic_flt = k_flt;
                                const float *ic_src = k_src;
                                int64_t ic = icb;
                                while (ic > 0) {
                                    --ic;
                                    IC_COMPUTE_STEP(0);
                                    ic_flt += CH_DT_BLK();
                                    ic_src += 1;
                                }
                            }
                            k_flt += CH_DT_BLK() * CH_DT_BLK();
                            k_src += src_dw_stride;
                            --kw;
                        } while (kw > 0);
                        k_src += src_dh_stride;
                        --kh;
                    } while (kh > 0);
                    icb_src += src_icb_stride;
                    icb -= CH_DT_BLK();
                } while (icb > 0);
            }
            { // session - finalize
                const uint64_t flags = PICK_PARAM(const uint64_t, shar_param, FLAGS_IDX());
                if (flags & (KERNEL_FLAG_RELU() | KERNEL_FLAG_RELU6())) {
                    ymm8 = _mm256_setzero_ps();
                    if (oc_len > 0 * OC_RF_BLK()) {
                        if (w_len > 0) ymm0 = _mm256_max_ps(ymm8, ymm0);
                        if (w_len > 1) ymm1 = _mm256_max_ps(ymm8, ymm1);
                        if (w_len > 2) ymm2 = _mm256_max_ps(ymm8, ymm2);
                        if (w_len > 3) ymm3 = _mm256_max_ps(ymm8, ymm3);
                        if (w_len > 4) ymm4 = _mm256_max_ps(ymm8, ymm4);
                        if (w_len > 5) ymm5 = _mm256_max_ps(ymm8, ymm5);
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        if (w_len > 0) ymm10 = _mm256_max_ps(ymm8, ymm10);
                        if (w_len > 1) ymm11 = _mm256_max_ps(ymm8, ymm11);
                        if (w_len > 2) ymm12 = _mm256_max_ps(ymm8, ymm12);
                        if (w_len > 3) ymm13 = _mm256_max_ps(ymm8, ymm13);
                        if (w_len > 4) ymm14 = _mm256_max_ps(ymm8, ymm14);
                        if (w_len > 5) ymm15 = _mm256_max_ps(ymm8, ymm15);
                    }
                }
                if (flags & KERNEL_FLAG_RELU6()) {
                    ymm9 = _mm256_set1_ps(6.0f);
                    if (oc_len > 0 * OC_RF_BLK()) {
                        if (w_len > 0) ymm0 = _mm256_min_ps(ymm9, ymm0);
                        if (w_len > 1) ymm1 = _mm256_min_ps(ymm9, ymm1);
                        if (w_len > 2) ymm2 = _mm256_min_ps(ymm9, ymm2);
                        if (w_len > 3) ymm3 = _mm256_min_ps(ymm9, ymm3);
                        if (w_len > 4) ymm4 = _mm256_min_ps(ymm9, ymm4);
                        if (w_len > 5) ymm5 = _mm256_min_ps(ymm9, ymm5);
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        if (w_len > 0) ymm10 = _mm256_min_ps(ymm9, ymm10);
                        if (w_len > 1) ymm11 = _mm256_min_ps(ymm9, ymm11);
                        if (w_len > 2) ymm12 = _mm256_min_ps(ymm9, ymm12);
                        if (w_len > 3) ymm13 = _mm256_min_ps(ymm9, ymm13);
                        if (w_len > 4) ymm14 = _mm256_min_ps(ymm9, ymm14);
                        if (w_len > 5) ymm15 = _mm256_min_ps(ymm9, ymm15);
                    }
                }
                if (nt_store) {
                    if (oc_len > 0 * OC_RF_BLK()) {
                        if (w_len > 0) _mm256_stream_ps(w_dst + 0 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm0);
                        if (w_len > 1) _mm256_stream_ps(w_dst + 1 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm1);
                        if (w_len > 2) _mm256_stream_ps(w_dst + 2 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm2);
                        if (w_len > 3) _mm256_stream_ps(w_dst + 3 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm3);
                        if (w_len > 4) _mm256_stream_ps(w_dst + 4 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm4);
                        if (w_len > 5) _mm256_stream_ps(w_dst + 5 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm5);
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        if (w_len > 0) _mm256_stream_ps(w_dst + 0 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm10);
                        if (w_len > 1) _mm256_stream_ps(w_dst + 1 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm11);
                        if (w_len > 2) _mm256_stream_ps(w_dst + 2 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm12);
                        if (w_len > 3) _mm256_stream_ps(w_dst + 3 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm13);
                        if (w_len > 4) _mm256_stream_ps(w_dst + 4 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm14);
                        if (w_len > 5) _mm256_stream_ps(w_dst + 5 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm15);
                    }
                } else {
                    if (oc_len > 0 * OC_RF_BLK()) {
                        if (w_len > 0) _mm256_storeu_ps(w_dst + 0 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm0);
                        if (w_len > 1) _mm256_storeu_ps(w_dst + 1 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm1);
                        if (w_len > 2) _mm256_storeu_ps(w_dst + 2 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm2);
                        if (w_len > 3) _mm256_storeu_ps(w_dst + 3 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm3);
                        if (w_len > 4) _mm256_storeu_ps(w_dst + 4 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm4);
                        if (w_len > 5) _mm256_storeu_ps(w_dst + 5 * CH_DT_BLK() + 0 * OC_RF_BLK(), ymm5);
                    }
                    if (oc_len > 1 * OC_RF_BLK()) {
                        if (w_len > 0) _mm256_storeu_ps(w_dst + 0 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm10);
                        if (w_len > 1) _mm256_storeu_ps(w_dst + 1 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm11);
                        if (w_len > 2) _mm256_storeu_ps(w_dst + 2 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm12);
                        if (w_len > 3) _mm256_storeu_ps(w_dst + 3 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm13);
                        if (w_len > 4) _mm256_storeu_ps(w_dst + 4 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm14);
                        if (w_len > 5) _mm256_storeu_ps(w_dst + 5 * CH_DT_BLK() + 1 * OC_RF_BLK(), ymm15);
                    }
                }
            }
            { // next ow block
                w_src += w_len * stride_w * CH_DT_BLK();
                w_his += w_len * CH_DT_BLK();
                w_dst += w_len * CH_DT_BLK();
                ow -= w_len;
            }
        } while (ow > 0);
        { // next oh
            h_src += src_sh_stride;
            h_his += PICK_PARAM(const int64_t, shar_param, HIS_H_STRIDE_IDX());
            h_dst += PICK_PARAM(const int64_t, shar_param, DST_H_STRIDE_IDX());
            oh -= 1;
        }
    } while (oh > 0);
#undef IC_COMPUTE_STEP
#undef IC_PREFETCH_STEP

#ifdef DO_PREFETCH_FLT
#undef DO_PREFETCH_FLT
#endif

#ifdef DO_PREFETCH_SRC
#undef DO_PREFETCH_SRC
#endif
}

#define DIRECT_BLK1X6_KERNEL_TABLE_BLK(STRIDE_W, NT_STORE, PREF_SRC) \
{\
    {\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 1 * OC_RF_BLK(), 1>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 1 * OC_RF_BLK(), 2>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 1 * OC_RF_BLK(), 3>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 1 * OC_RF_BLK(), 4>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 1 * OC_RF_BLK(), 5>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 1 * OC_RF_BLK(), 6>,\
    },\
    {\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 2 * OC_RF_BLK(), 1>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 2 * OC_RF_BLK(), 2>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 2 * OC_RF_BLK(), 3>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 2 * OC_RF_BLK(), 4>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 2 * OC_RF_BLK(), 5>,\
        conv2d_n16cx_direct_fp32_fma_blk1x6_kernel<STRIDE_W, NT_STORE, PREF_SRC, 2 * OC_RF_BLK(), 6>,\
    },\
}

conv2d_n16cx_direct_kernel_fp32_fma_func_t
conv2d_n16cx_direct_kernel_fp32_fma_blk1x6_table[STRIDE_W_OPT()][NT_STORE_OPT()][PREF_SRC_OPT()][BLK1X6_OC_RF()][BLK1X6_OW_RF()] =
{
    {
        {
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(1, false, false),
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(1, false, true),
        },
        {
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(1, true, false),
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(1, true, true),
        },
    },
    {
        {
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(2, false, false),
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(2, false, true),
        },
        {
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(2, true, false),
            DIRECT_BLK1X6_KERNEL_TABLE_BLK(2, true, true),
        },
    },
};

}}};
