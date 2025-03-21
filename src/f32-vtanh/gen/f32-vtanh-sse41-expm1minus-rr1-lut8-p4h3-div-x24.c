// Auto-generated file. Do not edit!
//   Template: src/f32-vtanh/sse-expm1minus.c.in
//   Generator: tools/xngen
//
// Copyright 2023 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <smmintrin.h>

#include <xnnpack/common.h>
#include <xnnpack/microparams.h>
#include <xnnpack/vunary.h>


// Table of exp2(k / 8) values decremented (as integer) by (k << 20), k = 0..7
extern XNN_INTERNAL const uint32_t xnn_table_exp2minus_k_over_8[8];

void xnn_f32_vtanh_ukernel__sse41_expm1minus_rr1_lut8_p4h3_div_x24(
    size_t batch,
    const float* input,
    float* output,
    const union xnn_f32_tanh_params params[restrict XNN_MIN_ELEMENTS(1)]) XNN_OOB_READS
{
  assert(batch != 0);
  assert(batch % sizeof(float) == 0);
  assert(input != NULL);
  assert(output != NULL);

  const __m128 vsign_mask = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.sign_mask);
  const __m128 vsat_cutoff = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.sat_cutoff);
  const __m128 vlog2e = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.log2e);
  const __m128 vmagic_bias = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.magic_bias);
  const __m128i vindex_mask = _mm_load_si128((const __m128i*) params->sse_expm1minus_rr1_lut8_p4h3.index_mask);
  const __m128 vminus_ln2 = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.minus_ln2);
  const __m128 vc4 = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.c4);
  const __m128 vc3 = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.c3);
  const __m128 vc2 = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.c2);
  const __m128 vminus_two = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.minus_two);
  const __m128 vminus_one = _mm_load_ps(params->sse_expm1minus_rr1_lut8_p4h3.minus_one);

  for (; batch >= 24 * sizeof(float); batch -= 24 * sizeof(float)) {
    const __m128 vx0123 = _mm_loadu_ps(input);
    const __m128 vx4567 = _mm_loadu_ps(input + 4);
    const __m128 vx89AB = _mm_loadu_ps(input + 8);
    const __m128 vxCDEF = _mm_loadu_ps(input + 12);
    const __m128 vxGHIJ = _mm_loadu_ps(input + 16);
    const __m128 vxKLMN = _mm_loadu_ps(input + 20);
    input += 24;

    __m128 vz0123 = _mm_or_ps(vx0123, vsign_mask);
    __m128 vz4567 = _mm_or_ps(vx4567, vsign_mask);
    __m128 vz89AB = _mm_or_ps(vx89AB, vsign_mask);
    __m128 vzCDEF = _mm_or_ps(vxCDEF, vsign_mask);
    __m128 vzGHIJ = _mm_or_ps(vxGHIJ, vsign_mask);
    __m128 vzKLMN = _mm_or_ps(vxKLMN, vsign_mask);

    const __m128 vinvsignx0123 = _mm_xor_ps(vx0123, vz0123);
    const __m128 vinvsignx4567 = _mm_xor_ps(vx4567, vz4567);
    const __m128 vinvsignx89AB = _mm_xor_ps(vx89AB, vz89AB);
    const __m128 vinvsignxCDEF = _mm_xor_ps(vxCDEF, vzCDEF);
    const __m128 vinvsignxGHIJ = _mm_xor_ps(vxGHIJ, vzGHIJ);
    const __m128 vinvsignxKLMN = _mm_xor_ps(vxKLMN, vzKLMN);

    vz0123 = _mm_max_ps(vsat_cutoff, vz0123);
    vz4567 = _mm_max_ps(vsat_cutoff, vz4567);
    vz89AB = _mm_max_ps(vsat_cutoff, vz89AB);
    vzCDEF = _mm_max_ps(vsat_cutoff, vzCDEF);
    vzGHIJ = _mm_max_ps(vsat_cutoff, vzGHIJ);
    vzKLMN = _mm_max_ps(vsat_cutoff, vzKLMN);

    __m128 vn0123 = _mm_add_ps(_mm_mul_ps(vz0123, vlog2e), vmagic_bias);
    __m128 vn4567 = _mm_add_ps(_mm_mul_ps(vz4567, vlog2e), vmagic_bias);
    __m128 vn89AB = _mm_add_ps(_mm_mul_ps(vz89AB, vlog2e), vmagic_bias);
    __m128 vnCDEF = _mm_add_ps(_mm_mul_ps(vzCDEF, vlog2e), vmagic_bias);
    __m128 vnGHIJ = _mm_add_ps(_mm_mul_ps(vzGHIJ, vlog2e), vmagic_bias);
    __m128 vnKLMN = _mm_add_ps(_mm_mul_ps(vzKLMN, vlog2e), vmagic_bias);

    const __m128i ve0123 = _mm_slli_epi32(_mm_castps_si128(vn0123), 20);
    const __m128i ve4567 = _mm_slli_epi32(_mm_castps_si128(vn4567), 20);
    const __m128i ve89AB = _mm_slli_epi32(_mm_castps_si128(vn89AB), 20);
    const __m128i veCDEF = _mm_slli_epi32(_mm_castps_si128(vnCDEF), 20);
    const __m128i veGHIJ = _mm_slli_epi32(_mm_castps_si128(vnGHIJ), 20);
    const __m128i veKLMN = _mm_slli_epi32(_mm_castps_si128(vnKLMN), 20);

    #if XNN_ARCH_X86_64
      __m128i vidx0123 = _mm_and_si128(_mm_castps_si128(vn0123), vindex_mask);
      __m128i vidx4567 = _mm_and_si128(_mm_castps_si128(vn4567), vindex_mask);
      __m128i vidx89AB = _mm_and_si128(_mm_castps_si128(vn89AB), vindex_mask);
      __m128i vidxCDEF = _mm_and_si128(_mm_castps_si128(vnCDEF), vindex_mask);
      __m128i vidxGHIJ = _mm_and_si128(_mm_castps_si128(vnGHIJ), vindex_mask);
      __m128i vidxKLMN = _mm_and_si128(_mm_castps_si128(vnKLMN), vindex_mask);

      const uint64_t vidx01 = (uint64_t) _mm_cvtsi128_si64(vidx0123);
      vidx0123 = _mm_unpackhi_epi64(vidx0123, vidx0123);
      const uint64_t vidx45 = (uint64_t) _mm_cvtsi128_si64(vidx4567);
      vidx4567 = _mm_unpackhi_epi64(vidx4567, vidx4567);
      const uint64_t vidx89 = (uint64_t) _mm_cvtsi128_si64(vidx89AB);
      vidx89AB = _mm_unpackhi_epi64(vidx89AB, vidx89AB);
      const uint64_t vidxCD = (uint64_t) _mm_cvtsi128_si64(vidxCDEF);
      vidxCDEF = _mm_unpackhi_epi64(vidxCDEF, vidxCDEF);
      const uint64_t vidxGH = (uint64_t) _mm_cvtsi128_si64(vidxGHIJ);
      vidxGHIJ = _mm_unpackhi_epi64(vidxGHIJ, vidxGHIJ);
      const uint64_t vidxKL = (uint64_t) _mm_cvtsi128_si64(vidxKLMN);
      vidxKLMN = _mm_unpackhi_epi64(vidxKLMN, vidxKLMN);

      const uint64_t vidx23 = (uint64_t) _mm_cvtsi128_si64(vidx0123);
      const uint64_t vidx67 = (uint64_t) _mm_cvtsi128_si64(vidx4567);
      const uint64_t vidxAB = (uint64_t) _mm_cvtsi128_si64(vidx89AB);
      const uint64_t vidxEF = (uint64_t) _mm_cvtsi128_si64(vidxCDEF);
      const uint64_t vidxIJ = (uint64_t) _mm_cvtsi128_si64(vidxGHIJ);
      const uint64_t vidxMN = (uint64_t) _mm_cvtsi128_si64(vidxKLMN);

      __m128i vl0123 = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx01]);
      vl0123 = _mm_insert_epi32(vl0123, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx01 >> 32)], 1);
      vl0123 = _mm_insert_epi32(vl0123, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx23], 2);
      vl0123 = _mm_insert_epi32(vl0123, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx23 >> 32)], 3);
      __m128i vl4567 = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx45]);
      vl4567 = _mm_insert_epi32(vl4567, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx45 >> 32)], 1);
      vl4567 = _mm_insert_epi32(vl4567, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx67], 2);
      vl4567 = _mm_insert_epi32(vl4567, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx67 >> 32)], 3);
      __m128i vl89AB = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx89]);
      vl89AB = _mm_insert_epi32(vl89AB, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx89 >> 32)], 1);
      vl89AB = _mm_insert_epi32(vl89AB, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxAB], 2);
      vl89AB = _mm_insert_epi32(vl89AB, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxAB >> 32)], 3);
      __m128i vlCDEF = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxCD]);
      vlCDEF = _mm_insert_epi32(vlCDEF, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxCD >> 32)], 1);
      vlCDEF = _mm_insert_epi32(vlCDEF, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxEF], 2);
      vlCDEF = _mm_insert_epi32(vlCDEF, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxEF >> 32)], 3);
      __m128i vlGHIJ = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxGH]);
      vlGHIJ = _mm_insert_epi32(vlGHIJ, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxGH >> 32)], 1);
      vlGHIJ = _mm_insert_epi32(vlGHIJ, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxIJ], 2);
      vlGHIJ = _mm_insert_epi32(vlGHIJ, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxIJ >> 32)], 3);
      __m128i vlKLMN = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxKL]);
      vlKLMN = _mm_insert_epi32(vlKLMN, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxKL >> 32)], 1);
      vlKLMN = _mm_insert_epi32(vlKLMN, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidxMN], 2);
      vlKLMN = _mm_insert_epi32(vlKLMN, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidxMN >> 32)], 3);
    #else
      const __m128i vidx0123 = _mm_and_si128(_mm_castps_si128(vn0123), vindex_mask);
      const __m128i vidx4567 = _mm_and_si128(_mm_castps_si128(vn4567), vindex_mask);
      const __m128i vidx89AB = _mm_and_si128(_mm_castps_si128(vn89AB), vindex_mask);
      const __m128i vidxCDEF = _mm_and_si128(_mm_castps_si128(vnCDEF), vindex_mask);
      const __m128i vidxGHIJ = _mm_and_si128(_mm_castps_si128(vnGHIJ), vindex_mask);
      const __m128i vidxKLMN = _mm_and_si128(_mm_castps_si128(vnKLMN), vindex_mask);

      const uint32_t vidx0 = (uint32_t) _mm_cvtsi128_si32(vidx0123);
      const uint32_t vidx4 = (uint32_t) _mm_cvtsi128_si32(vidx4567);
      const uint32_t vidx8 = (uint32_t) _mm_cvtsi128_si32(vidx89AB);
      const uint32_t vidxC = (uint32_t) _mm_cvtsi128_si32(vidxCDEF);
      const uint32_t vidxG = (uint32_t) _mm_cvtsi128_si32(vidxGHIJ);
      const uint32_t vidxK = (uint32_t) _mm_cvtsi128_si32(vidxKLMN);

      __m128i vl0123 = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidx0]);
      __m128i vl4567 = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidx4]);
      __m128i vl89AB = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidx8]);
      __m128i vlCDEF = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidxC]);
      __m128i vlGHIJ = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidxG]);
      __m128i vlKLMN = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidxK]);

      const uint32_t vidx1 = (uint32_t) _mm_extract_epi16(vidx0123, 2);
      const uint32_t vidx5 = (uint32_t) _mm_extract_epi16(vidx4567, 2);
      const uint32_t vidx9 = (uint32_t) _mm_extract_epi16(vidx89AB, 2);
      const uint32_t vidxD = (uint32_t) _mm_extract_epi16(vidxCDEF, 2);
      const uint32_t vidxH = (uint32_t) _mm_extract_epi16(vidxGHIJ, 2);
      const uint32_t vidxL = (uint32_t) _mm_extract_epi16(vidxKLMN, 2);

      vl0123 = _mm_insert_epi32(vl0123, (int) xnn_table_exp2minus_k_over_8[vidx1], 1);
      vl4567 = _mm_insert_epi32(vl4567, (int) xnn_table_exp2minus_k_over_8[vidx5], 1);
      vl89AB = _mm_insert_epi32(vl89AB, (int) xnn_table_exp2minus_k_over_8[vidx9], 1);
      vlCDEF = _mm_insert_epi32(vlCDEF, (int) xnn_table_exp2minus_k_over_8[vidxD], 1);
      vlGHIJ = _mm_insert_epi32(vlGHIJ, (int) xnn_table_exp2minus_k_over_8[vidxH], 1);
      vlKLMN = _mm_insert_epi32(vlKLMN, (int) xnn_table_exp2minus_k_over_8[vidxL], 1);

      const uint32_t vidx2 = (uint32_t) _mm_extract_epi16(vidx0123, 4);
      const uint32_t vidx6 = (uint32_t) _mm_extract_epi16(vidx4567, 4);
      const uint32_t vidxA = (uint32_t) _mm_extract_epi16(vidx89AB, 4);
      const uint32_t vidxE = (uint32_t) _mm_extract_epi16(vidxCDEF, 4);
      const uint32_t vidxI = (uint32_t) _mm_extract_epi16(vidxGHIJ, 4);
      const uint32_t vidxM = (uint32_t) _mm_extract_epi16(vidxKLMN, 4);

      vl0123 = _mm_insert_epi32(vl0123, (int) xnn_table_exp2minus_k_over_8[vidx2], 2);
      vl4567 = _mm_insert_epi32(vl4567, (int) xnn_table_exp2minus_k_over_8[vidx6], 2);
      vl89AB = _mm_insert_epi32(vl89AB, (int) xnn_table_exp2minus_k_over_8[vidxA], 2);
      vlCDEF = _mm_insert_epi32(vlCDEF, (int) xnn_table_exp2minus_k_over_8[vidxE], 2);
      vlGHIJ = _mm_insert_epi32(vlGHIJ, (int) xnn_table_exp2minus_k_over_8[vidxI], 2);
      vlKLMN = _mm_insert_epi32(vlKLMN, (int) xnn_table_exp2minus_k_over_8[vidxM], 2);

      const uint32_t vidx3 = (uint32_t) _mm_extract_epi16(vidx0123, 6);
      const uint32_t vidx7 = (uint32_t) _mm_extract_epi16(vidx4567, 6);
      const uint32_t vidxB = (uint32_t) _mm_extract_epi16(vidx89AB, 6);
      const uint32_t vidxF = (uint32_t) _mm_extract_epi16(vidxCDEF, 6);
      const uint32_t vidxJ = (uint32_t) _mm_extract_epi16(vidxGHIJ, 6);
      const uint32_t vidxN = (uint32_t) _mm_extract_epi16(vidxKLMN, 6);

      vl0123 = _mm_insert_epi32(vl0123, (int) xnn_table_exp2minus_k_over_8[vidx3], 3);
      vl4567 = _mm_insert_epi32(vl4567, (int) xnn_table_exp2minus_k_over_8[vidx7], 3);
      vl89AB = _mm_insert_epi32(vl89AB, (int) xnn_table_exp2minus_k_over_8[vidxB], 3);
      vlCDEF = _mm_insert_epi32(vlCDEF, (int) xnn_table_exp2minus_k_over_8[vidxF], 3);
      vlGHIJ = _mm_insert_epi32(vlGHIJ, (int) xnn_table_exp2minus_k_over_8[vidxJ], 3);
      vlKLMN = _mm_insert_epi32(vlKLMN, (int) xnn_table_exp2minus_k_over_8[vidxN], 3);
    #endif

    const __m128 vs0123 = _mm_castsi128_ps(_mm_add_epi32(vl0123, ve0123));
    const __m128 vs4567 = _mm_castsi128_ps(_mm_add_epi32(vl4567, ve4567));
    const __m128 vs89AB = _mm_castsi128_ps(_mm_add_epi32(vl89AB, ve89AB));
    const __m128 vsCDEF = _mm_castsi128_ps(_mm_add_epi32(vlCDEF, veCDEF));
    const __m128 vsGHIJ = _mm_castsi128_ps(_mm_add_epi32(vlGHIJ, veGHIJ));
    const __m128 vsKLMN = _mm_castsi128_ps(_mm_add_epi32(vlKLMN, veKLMN));

    vn0123 = _mm_sub_ps(vn0123, vmagic_bias);
    vn4567 = _mm_sub_ps(vn4567, vmagic_bias);
    vn89AB = _mm_sub_ps(vn89AB, vmagic_bias);
    vnCDEF = _mm_sub_ps(vnCDEF, vmagic_bias);
    vnGHIJ = _mm_sub_ps(vnGHIJ, vmagic_bias);
    vnKLMN = _mm_sub_ps(vnKLMN, vmagic_bias);

    const __m128 vt0123 = _mm_add_ps(_mm_mul_ps(vn0123, vminus_ln2), vz0123);
    const __m128 vt4567 = _mm_add_ps(_mm_mul_ps(vn4567, vminus_ln2), vz4567);
    const __m128 vt89AB = _mm_add_ps(_mm_mul_ps(vn89AB, vminus_ln2), vz89AB);
    const __m128 vtCDEF = _mm_add_ps(_mm_mul_ps(vnCDEF, vminus_ln2), vzCDEF);
    const __m128 vtGHIJ = _mm_add_ps(_mm_mul_ps(vnGHIJ, vminus_ln2), vzGHIJ);
    const __m128 vtKLMN = _mm_add_ps(_mm_mul_ps(vnKLMN, vminus_ln2), vzKLMN);

    __m128 vp0123 = _mm_add_ps(_mm_mul_ps(vc4, vt0123), vc3);
    __m128 vp4567 = _mm_add_ps(_mm_mul_ps(vc4, vt4567), vc3);
    __m128 vp89AB = _mm_add_ps(_mm_mul_ps(vc4, vt89AB), vc3);
    __m128 vpCDEF = _mm_add_ps(_mm_mul_ps(vc4, vtCDEF), vc3);
    __m128 vpGHIJ = _mm_add_ps(_mm_mul_ps(vc4, vtGHIJ), vc3);
    __m128 vpKLMN = _mm_add_ps(_mm_mul_ps(vc4, vtKLMN), vc3);
    vp0123 = _mm_add_ps(_mm_mul_ps(vp0123, vt0123), vc2);
    vp4567 = _mm_add_ps(_mm_mul_ps(vp4567, vt4567), vc2);
    vp89AB = _mm_add_ps(_mm_mul_ps(vp89AB, vt89AB), vc2);
    vpCDEF = _mm_add_ps(_mm_mul_ps(vpCDEF, vtCDEF), vc2);
    vpGHIJ = _mm_add_ps(_mm_mul_ps(vpGHIJ, vtGHIJ), vc2);
    vpKLMN = _mm_add_ps(_mm_mul_ps(vpKLMN, vtKLMN), vc2);
    vp0123 = _mm_sub_ps(_mm_mul_ps(vp0123, vt0123), vminus_two);
    vp4567 = _mm_sub_ps(_mm_mul_ps(vp4567, vt4567), vminus_two);
    vp89AB = _mm_sub_ps(_mm_mul_ps(vp89AB, vt89AB), vminus_two);
    vpCDEF = _mm_sub_ps(_mm_mul_ps(vpCDEF, vtCDEF), vminus_two);
    vpGHIJ = _mm_sub_ps(_mm_mul_ps(vpGHIJ, vtGHIJ), vminus_two);
    vpKLMN = _mm_sub_ps(_mm_mul_ps(vpKLMN, vtKLMN), vminus_two);

    const __m128 vts0123 = _mm_mul_ps(vt0123, vs0123);
    const __m128 vsmo0123 = _mm_add_ps(vs0123, vminus_one);
    const __m128 vts4567 = _mm_mul_ps(vt4567, vs4567);
    const __m128 vsmo4567 = _mm_add_ps(vs4567, vminus_one);
    const __m128 vts89AB = _mm_mul_ps(vt89AB, vs89AB);
    const __m128 vsmo89AB = _mm_add_ps(vs89AB, vminus_one);
    const __m128 vtsCDEF = _mm_mul_ps(vtCDEF, vsCDEF);
    const __m128 vsmoCDEF = _mm_add_ps(vsCDEF, vminus_one);
    const __m128 vtsGHIJ = _mm_mul_ps(vtGHIJ, vsGHIJ);
    const __m128 vsmoGHIJ = _mm_add_ps(vsGHIJ, vminus_one);
    const __m128 vtsKLMN = _mm_mul_ps(vtKLMN, vsKLMN);
    const __m128 vsmoKLMN = _mm_add_ps(vsKLMN, vminus_one);
    const __m128 vemo0123 = _mm_add_ps(_mm_mul_ps(vp0123, vts0123), vsmo0123);
    const __m128 vemo4567 = _mm_add_ps(_mm_mul_ps(vp4567, vts4567), vsmo4567);
    const __m128 vemo89AB = _mm_add_ps(_mm_mul_ps(vp89AB, vts89AB), vsmo89AB);
    const __m128 vemoCDEF = _mm_add_ps(_mm_mul_ps(vpCDEF, vtsCDEF), vsmoCDEF);
    const __m128 vemoGHIJ = _mm_add_ps(_mm_mul_ps(vpGHIJ, vtsGHIJ), vsmoGHIJ);
    const __m128 vemoKLMN = _mm_add_ps(_mm_mul_ps(vpKLMN, vtsKLMN), vsmoKLMN);

    const __m128 vepo0123 = _mm_sub_ps(vemo0123, vminus_two);
    const __m128 vepo4567 = _mm_sub_ps(vemo4567, vminus_two);
    const __m128 vepo89AB = _mm_sub_ps(vemo89AB, vminus_two);
    const __m128 vepoCDEF = _mm_sub_ps(vemoCDEF, vminus_two);
    const __m128 vepoGHIJ = _mm_sub_ps(vemoGHIJ, vminus_two);
    const __m128 vepoKLMN = _mm_sub_ps(vemoKLMN, vminus_two);

    __m128 vy0123 = _mm_div_ps(vemo0123, vepo0123);
    __m128 vy4567 = _mm_div_ps(vemo4567, vepo4567);
    __m128 vy89AB = _mm_div_ps(vemo89AB, vepo89AB);
    __m128 vyCDEF = _mm_div_ps(vemoCDEF, vepoCDEF);
    __m128 vyGHIJ = _mm_div_ps(vemoGHIJ, vepoGHIJ);
    __m128 vyKLMN = _mm_div_ps(vemoKLMN, vepoKLMN);


    vy0123 = _mm_xor_ps(vy0123, vinvsignx0123);
    vy4567 = _mm_xor_ps(vy4567, vinvsignx4567);
    vy89AB = _mm_xor_ps(vy89AB, vinvsignx89AB);
    vyCDEF = _mm_xor_ps(vyCDEF, vinvsignxCDEF);
    vyGHIJ = _mm_xor_ps(vyGHIJ, vinvsignxGHIJ);
    vyKLMN = _mm_xor_ps(vyKLMN, vinvsignxKLMN);

    _mm_storeu_ps(output, vy0123);
    _mm_storeu_ps(output + 4, vy4567);
    _mm_storeu_ps(output + 8, vy89AB);
    _mm_storeu_ps(output + 12, vyCDEF);
    _mm_storeu_ps(output + 16, vyGHIJ);
    _mm_storeu_ps(output + 20, vyKLMN);
    output += 24;
  }
  for (; batch >= 4 * sizeof(float); batch -= 4 * sizeof(float)) {
    const __m128 vx = _mm_loadu_ps(input);
    input += 4;

    __m128 vz = _mm_or_ps(vx, vsign_mask);

    const __m128 vinvsignx = _mm_xor_ps(vx, vz);

    vz = _mm_max_ps(vsat_cutoff, vz);

    __m128 vn = _mm_add_ps(_mm_mul_ps(vz, vlog2e), vmagic_bias);

    const __m128i ve = _mm_slli_epi32(_mm_castps_si128(vn), 20);

    #if XNN_ARCH_X86_64
      __m128i vidx = _mm_and_si128(_mm_castps_si128(vn), vindex_mask);
      const uint64_t vidx_lo = (uint64_t) _mm_cvtsi128_si64(vidx);
      vidx = _mm_unpackhi_epi64(vidx, vidx);
      const uint64_t vidx_hi = (uint64_t) _mm_cvtsi128_si64(vidx);
      __m128i vl = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx_lo]);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx_lo >> 32)], 1);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx_hi], 2);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx_hi >> 32)], 3);
    #else
      const __m128i vidx = _mm_and_si128(_mm_castps_si128(vn), vindex_mask);
      const uint32_t vidx0 = (uint32_t) _mm_cvtsi128_si32(vidx);
      __m128i vl = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidx0]);
      const uint32_t vidx1 = (uint32_t) _mm_extract_epi16(vidx, 2);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[vidx1], 1);
      const uint32_t vidx2 = (uint32_t) _mm_extract_epi16(vidx, 4);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[vidx2], 2);
      const uint32_t vidx3 = (uint32_t) _mm_extract_epi16(vidx, 6);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[vidx3], 3);
    #endif

    const __m128 vs = _mm_castsi128_ps(_mm_add_epi32(vl, ve));

    vn = _mm_sub_ps(vn, vmagic_bias);

    const __m128 vt = _mm_add_ps(_mm_mul_ps(vn, vminus_ln2), vz);

    __m128 vp = _mm_add_ps(_mm_mul_ps(vc4, vt), vc3);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc2);
    vp = _mm_sub_ps(_mm_mul_ps(vp, vt), vminus_two);

    const __m128 vts = _mm_mul_ps(vt, vs);
    const __m128 vsmo = _mm_add_ps(vs, vminus_one);
    const __m128 vemo = _mm_add_ps(_mm_mul_ps(vp, vts), vsmo);

    const __m128 vepo = _mm_sub_ps(vemo, vminus_two);

    __m128 vy = _mm_div_ps(vemo, vepo);


    vy = _mm_xor_ps(vy, vinvsignx);

    _mm_storeu_ps(output, vy);
    output += 4;
  }
  if XNN_UNLIKELY(batch != 0) {
    const __m128 vx = _mm_loadu_ps(input);

    __m128 vz = _mm_or_ps(vx, vsign_mask);

    const __m128 vinvsignx = _mm_xor_ps(vx, vz);

    vz = _mm_max_ps(vsat_cutoff, vz);

    __m128 vn = _mm_add_ps(_mm_mul_ps(vz, vlog2e), vmagic_bias);

    const __m128i ve = _mm_slli_epi32(_mm_castps_si128(vn), 20);

    #if XNN_ARCH_X86_64
      __m128i vidx = _mm_and_si128(_mm_castps_si128(vn), vindex_mask);
      const uint64_t vidx_lo = (uint64_t) _mm_cvtsi128_si64(vidx);
      vidx = _mm_unpackhi_epi64(vidx, vidx);
      const uint64_t vidx_hi = (uint64_t) _mm_cvtsi128_si64(vidx);
      __m128i vl = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx_lo]);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx_lo >> 32)], 1);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[(uint32_t) vidx_hi], 2);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[(uint32_t) (vidx_hi >> 32)], 3);
    #else
      const __m128i vidx = _mm_and_si128(_mm_castps_si128(vn), vindex_mask);
      const uint32_t vidx0 = (uint32_t) _mm_cvtsi128_si32(vidx);
      __m128i vl = _mm_cvtsi32_si128((int) xnn_table_exp2minus_k_over_8[vidx0]);
      const uint32_t vidx1 = (uint32_t) _mm_extract_epi16(vidx, 2);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[vidx1], 1);
      const uint32_t vidx2 = (uint32_t) _mm_extract_epi16(vidx, 4);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[vidx2], 2);
      const uint32_t vidx3 = (uint32_t) _mm_extract_epi16(vidx, 6);
      vl = _mm_insert_epi32(vl, (int) xnn_table_exp2minus_k_over_8[vidx3], 3);
    #endif

    const __m128 vs = _mm_castsi128_ps(_mm_add_epi32(vl, ve));

    vn = _mm_sub_ps(vn, vmagic_bias);

    const __m128 vt = _mm_add_ps(_mm_mul_ps(vn, vminus_ln2), vz);

    __m128 vp = _mm_add_ps(_mm_mul_ps(vc4, vt), vc3);
    vp = _mm_add_ps(_mm_mul_ps(vp, vt), vc2);
    vp = _mm_sub_ps(_mm_mul_ps(vp, vt), vminus_two);

    const __m128 vts = _mm_mul_ps(vt, vs);
    const __m128 vsmo = _mm_add_ps(vs, vminus_one);
    const __m128 vemo = _mm_add_ps(_mm_mul_ps(vp, vts), vsmo);

    const __m128 vepo = _mm_sub_ps(vemo, vminus_two);

    __m128 vy = _mm_div_ps(vemo, vepo);


    vy = _mm_xor_ps(vy, vinvsignx);

    if (batch & (2 * sizeof(float))) {
      _mm_storel_pi((__m64*) output, vy);
      vy = _mm_movehl_ps(vy, vy);
      output += 2;
    }
    if (batch & (1 * sizeof(float))) {
      _mm_store_ss(output, vy);
    }
  }
}
