// Auto-generated file. Do not edit!
//   Template: src/qs8-dwconv/unipass-scalar.c.in
//   Generator: tools/xngen
//
// Copyright 2021 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <assert.h>

#include <fp16.h>

#include <xnnpack/dwconv.h>
#include <xnnpack/math.h>


void xnn_qc8_dwconv_minmax_fp32_ukernel_up2x3__scalar_imagic(
    size_t channels,
    size_t output_width,
    const int8_t** input,
    const void* weights,
    int8_t* output,
    size_t input_stride,
    size_t output_increment,
    size_t input_offset,
    const int8_t* zero,
    const union xnn_qs8_minmax_params params[restrict XNN_MIN_ELEMENTS(1)])
{
  assert(channels != 0);
  assert(output_width != 0);

  const float vmagic_bias = params->scalar_imagic.magic_bias;
  const int32_t vmagic_min = params->scalar_imagic.magic_min;
  const int32_t vmagic_max = params->scalar_imagic.magic_max;
  const int32_t vmagic_bias_less_zero_point = params->scalar_imagic.magic_bias_less_zero_point;
  do {
    const int8_t* i0 = input[0];
    assert(i0 != NULL);
    if XNN_UNPREDICTABLE(i0 != zero) {
      i0 = (const int8_t*) ((uintptr_t) i0 + input_offset);
    }
    const int8_t* i1 = input[1];
    assert(i1 != NULL);
    if XNN_UNPREDICTABLE(i1 != zero) {
      i1 = (const int8_t*) ((uintptr_t) i1 + input_offset);
    }
    const int8_t* i2 = input[2];
    assert(i2 != NULL);
    if XNN_UNPREDICTABLE(i2 != zero) {
      i2 = (const int8_t*) ((uintptr_t) i2 + input_offset);
    }
    input = (const int8_t**) ((uintptr_t) input + input_stride);

    size_t c = channels;
    const void* w = weights;
    for (; c >= 2; c -= 2) {
      int32_t vacc0 = ((const int32_t*) w)[0];
      int32_t vacc1 = ((const int32_t*) w)[1];


      const int32_t vi0x0 = (int32_t) i0[0];
      const int32_t vi0x1 = (int32_t) i0[1];
      i0 += 2;

      const int32_t vk0x0 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[0];
      const int32_t vk0x1 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[1];

      vacc0 += vi0x0 * vk0x0;
      vacc1 += vi0x1 * vk0x1;

      const int32_t vi1x0 = (int32_t) i1[0];
      const int32_t vi1x1 = (int32_t) i1[1];
      i1 += 2;

      const int32_t vk1x0 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[2];
      const int32_t vk1x1 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[3];

      vacc0 += vi1x0 * vk1x0;
      vacc1 += vi1x1 * vk1x1;

      const int32_t vi2x0 = (int32_t) i2[0];
      const int32_t vi2x1 = (int32_t) i2[1];
      i2 += 2;

      const int32_t vk2x0 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[4];
      const int32_t vk2x1 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[5];

      vacc0 += vi2x0 * vk2x0;
      vacc1 += vi2x1 * vk2x1;

      w = (const void*) ((uintptr_t) w + 2 * sizeof(int32_t) + 6 * sizeof(int8_t));

      float vfpacc0 = (float) vacc0;
      float vfpacc1 = (float) vacc1;

      typedef XNN_UNALIGNED float unaligned_float;
      const float vscale0 = ((const unaligned_float*) w)[0];
      const float vscale1 = ((const unaligned_float*) w)[1];
      w = (const void*) ((const float*) w + 2);

      vfpacc0 *= vscale0;
      vfpacc1 *= vscale1;

      vfpacc0 += vmagic_bias;
      vfpacc1 += vmagic_bias;

      int32_t vout0 = (int32_t) fp32_to_bits(vfpacc0);
      int32_t vout1 = (int32_t) fp32_to_bits(vfpacc1);

      vout0 = math_max_s32(vout0, vmagic_min);
      vout1 = math_max_s32(vout1, vmagic_min);

      vout0 = math_min_s32(vout0, vmagic_max);
      vout1 = math_min_s32(vout1, vmagic_max);

      vout0 -= vmagic_bias_less_zero_point;
      vout1 -= vmagic_bias_less_zero_point;

      output[0] = (int8_t) vout0;
      output[1] = (int8_t) vout1;
      output += 2;
    }
    if XNN_UNLIKELY(c != 0) {
      int32_t vacc = *((const int32_t*) w);

      const int32_t vi0 = (int32_t) *i0;
      const int32_t vk0 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[0];
      vacc += vi0 * vk0;
      const int32_t vi1 = (int32_t) *i1;
      const int32_t vk1 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[2];
      vacc += vi1 * vk1;
      const int32_t vi2 = (int32_t) *i2;
      const int32_t vk2 = (int32_t) ((const int8_t*) ((uintptr_t) w + 2 * sizeof(int32_t)))[4];
      vacc += vi2 * vk2;

      typedef XNN_UNALIGNED float unaligned_float;
      const float vscale = *((const unaligned_float*) ((uintptr_t) w + 2 * sizeof(int32_t) + 6 * sizeof(int8_t)));
      float vfpacc = (float) vacc * vscale;

      vfpacc += vmagic_bias;
      int32_t vout = (int32_t) fp32_to_bits(vfpacc);
      vout = math_max_s32(vout, vmagic_min);
      vout = math_min_s32(vout, vmagic_max);
      vout -= vmagic_bias_less_zero_point;

      *output++ = (int8_t) vout;
    }

    output = (int8_t*) ((uintptr_t) output + output_increment);
  } while (--output_width != 0);
}