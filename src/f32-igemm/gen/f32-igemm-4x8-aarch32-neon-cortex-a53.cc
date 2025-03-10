// Copyright 2019 Google LLC
//
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree.

#include <cassert>
#include <cstddef>
#include <limits>

#include <xnnpack.h>
#include <xnnpack/aarch32-assembler.h>
#include <xnnpack/igemm.h>
#include <xnnpack/memory.h>
#include <xnnpack/microparams.h>
#include <xnnpack/post-operation.h>

namespace xnnpack {
namespace aarch32 {
namespace {
class Generator : public MacroAssembler {
  using MacroAssembler::MacroAssembler;

 public:
  void generate(bool prefetch, size_t max_mr, size_t nc_mod_nr, size_t kc, size_t ks, const jit_gemm_params* jit_gemm_params);
  void perform_post_operations(size_t max_mr, size_t num_post_operations, const xnn_post_operation* post_operations);
};


// void xnn_f32_igemm_minmax_ukernel_4x8__asm_aarch32_neon_prfm_cortex_a53(
//     size_t mr,                            r0
//     size_t nc,                            r1
//     size_t kc,                            r2 -> r5 -> sp + 68
//     size_t ks,                            r3 -> sp + 72 -> r14
//     const float**restrict a,  sp + 112 -> (r5)
//     const void*restrict w,    sp + 116 -> r9
//     uint8_t*restrict c,       sp + 120 -> r11
//     size_t cm_stride,         sp + 124 -> (r6)
//     size_t cn_stride,         sp + 128 -> (r0)
//     size_t a_offset,          sp + 132 -> (r5)
//     const float* zero,        sp + 136 -> (r0)
//     minmax_params*params,     sp + 140 -> (r2)

// d8-d15, r4-r11,r14(lr) need to be preserved if used. r13(sp),r15(pc) are reserved.

// Register usage
// A0   r3  d0 d4
// A1  r12  d1 d5
// A2  r10  d2 d6
// A3   r7  d3 d7
// B    r9  d8,  d9, d10, d11
// B       d12, d13, d14, d15
// C0  r11 d16-d17  q8  d18-d19  q9
// C1   r4 d20-d21 q10  d22-d23 q11
// C2   r8 d24-d25 q12  d26-d27 q13
// C3   r6 d28-d29 q14  d30-d31 q15
// clamp  (r2) d4 d5 d6 d7
// temp r0, r2 for Cortex-A53 loads

// Converted from: src/f32-igemm/gen/f32-igemm-4x8-minmax-asm-aarch32-neon-prfm-cortex-a53.S
void Generator::generate(bool prefetch, size_t max_mr, size_t nc_mod_nr, size_t kc, size_t ks, const jit_gemm_params* jit_gemm_params)
{
  assert(max_mr <= 4);
  assert(nc_mod_nr < 8);
  assert(kc != 0);
  assert(kc % sizeof(float) == 0);
  assert(ks != 0);

  Label l0, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10;
  const size_t num_post_operations = jit_gemm_params->num_post_operations;
  const xnn_post_operation* post_operations = jit_gemm_params->post_operations;
  const float min = jit_gemm_params->f32_minmax.min;
  const float max = jit_gemm_params->f32_minmax.max;
  const bool clamp_min = min != -std::numeric_limits<float>::infinity();
  const bool clamp_max = max != +std::numeric_limits<float>::infinity();
  assert(num_post_operations == 0 || (!clamp_min && !clamp_max));
  // Push 112 bytes
  // r2 will be reloaded in outer loop.  r3 is ks
  push({r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}); // +44
  sub(sp, sp, 4); // 4
  vpush({d8-d15}); // +64 = 112

  ldr(r11, mem[sp, 120]); // c
  ldr(r6, mem[sp, 124]); // cm_stride
  ldr(r5, mem[sp, 112]); // a
  ldr(r9, mem[sp, 116]); // w
  mov(r14, r3); // p = ks

  // Clamp C pointers
  if (max_mr > 1) {
    cmp(r0, 2); // if mr >= 2
    add(r4, r11, r6); //   c1 = c0 + cm_stride
    movlo(r4, r11); // c1
  }
  if (max_mr > 2) {
    // if mr > 2
    add(r8, r4, r6); //   c2 = c1 + cm_stride
    movls(r8, r4); // c2
  }
  if (max_mr > 3) {
    cmp(r0, 4); // if mr >=4
    add(r6, r8, r6); //   c3 = c2 + cm_stride
    movlo(r6, r8); // c3
  }


  align(8);
  bind(l0);
  // Load initial bias from w into accumulators
  vldm(mem[r9]++, {d16-d19}); // Bias

  if (max_mr > 1) {
    vmov(q10, q8);
  }
  if (prefetch) {
    pld(mem[r9, 0]); // Prefetch B
  }
  if (max_mr > 1) {
    vmov(q11, q9);
  }
  if (prefetch) {
    pld(mem[r9, 64]);
  }
  if (max_mr > 2) {
    vmov(q12, q8);
  }
  if (prefetch) {
    pld(mem[r9, 128]);
  }
  if (max_mr > 2) {
    vmov(q13, q9);
  }
  if (prefetch) {
    pld(mem[r9, 192]);
  }
  if (max_mr > 3) {
    vmov(q14, q8);
  }
  if (prefetch) {
    pld(mem[r9, 256]);
  }
  if (max_mr > 3) {
    vmov(q15, q9);
  }
  if (prefetch) {
    pld(mem[r9, 320]);
  }

  bind(l1);
  // Load next 4 A pointers
  ldr(r3, mem[r5, 0]);
  if (max_mr > 1) {
    ldr(r12, mem[r5, 4]);
  }
  if (max_mr > 2) {
    ldr(r10, mem[r5, 8]);
  }
  if (max_mr > 3) {
    ldr(r7, mem[r5, 12]);
  }
  add(r5, r5, max_mr * sizeof(void*)); // a += MR * sizeof(void*)
  if (prefetch) {
    pld(mem[r3, 0]); // Prefetch A
  }
  str(r5, mem[sp, 112]); // a
  if (prefetch) {
    pld(mem[r3, 64]);
  }
  ldr(r0, mem[sp, 136]); // zero
  if (prefetch) {
    pld(mem[r12, 0]);
  }
  ldr(r5, mem[sp, 132]); // a_offset
  if (prefetch) {
    pld(mem[r12, 64]);
  }
  ldr(r2, mem[sp, 68]); // kc
  if (prefetch) {
    pld(mem[r10, 0]);
    pld(mem[r10, 64]);
    pld(mem[r7, 0]);
    pld(mem[r7, 64]);
  }

  // Add a_offset
  cmp(r3, r0); // if a0 == zero
  add(r3, r3, r5); // a0 += a_offset
  moveq(r3, r0); //   a0 = zero, else += a0 + a_offset
  if (max_mr > 1) {
    cmp(r12, r0); // if a1 == zero
    add(r12, r12, r5); // a1 += a_offset
    moveq(r12, r0); //   a1 = zero, else += a1 + a_offset
  }
  if (max_mr > 2) {
    cmp(r10, r0); // if a2 == zero
    add(r10, r10, r5); // a2 += a_offset
    moveq(r10, r0); //   a2 = zero, else += a2 + a_offset
  }
  if (max_mr > 3) {
    cmp(r7, r0); // if a3 == zero
    add(r7, r7, r5); // a3 += a_offset
    moveq(r7, r0); //   a3 = zero, else += a3 + a_offset
  }

  subs(r5, r2, 16); // kc - 16
  blo(l5); // less than 4 channels?

  // Prologue
  vld1_32({d0}, mem[r3]++); // A0
  if (max_mr > 1) {
    vld1_32({d1}, mem[r12]++); // A1
  }
  if (max_mr > 2) {
    vld1_32({d2}, mem[r10]++); // A2
  }
  if (max_mr > 3) {
    vld1_32({d3}, mem[r7]++); // A3
  }
  subs(r5, r5, 16);
  vldm(mem[r9], {d8-d11}); // B0
  ldr(r0, mem[r9, 56]); // B1 low   VMOV is in BLOCK 0
  ldr(r2, mem[r9, 60]); // B1 high
  vldr(d13, mem[r9, 40]); // B1

  blo(l3); // less than 4 channels?  skip main loop

  // Main loop - 4 floats of A (16 bytes)
  // 32 FMA + 8 LD64 A + 8 LDR B
  align(8);
  bind(l2);
  // First group of 16 FMA, Second group loads
  // BLOCK 0
  vld1_32({d4}, mem[r3]++); // A0
  vmov(d15, r0, r2); // b1 VMOV b from second group
  vmla_f32(q8, q4, d0[0]);
  if (max_mr > 1) {
    ldr(r0, mem[r12]); // A1 low
    vmla_f32(q10, q4, d1[0]);
    ldr(r2, mem[r12, 4]); // A1 high
  }
  if (max_mr > 2) {
    vmla_f32(q12, q4, d2[0]);
  }
  if (prefetch) {
    pld(mem[r3, 128]); // Prefetch A0
  }

  // BLOCK 1
  vldr(d12, mem[r9, 32]); // B1
  if (max_mr > 1) {
    vmov(d5, r0, r2); // a1 VMOV
  }
  if (max_mr > 3) {
    vmla_f32(q14, q4, d3[0]);
  }
  ldr(r0, mem[r9, 72]); // B0 low
  vmla_f32(q9, q5, d0[0]);
  ldr(r2, mem[r9, 76]); // B0 high
  if (max_mr > 1) {
    vmla_f32(q11, q5, d1[0]);
  }
  if (prefetch) {
    pld(mem[r12, 128]); // Prefetch A1
  }

  // BLOCK 2
  if (max_mr > 2) {
    vld1_32({d6}, mem[r10]++); // A2
  }
  vmov(d9, r0, r2); // b0 VMOV
  if (max_mr > 2) {
    vmla_f32(q13, q5, d2[0]);
  }
  if (max_mr > 3) {
    ldr(r0, mem[r7]); // A3 low
    vmla_f32(q15, q5, d3[0]);
    ldr(r2, mem[r7, 4]); // A3 high
  }
  vmla_f32(q8, q6, d0[1]);
  if (prefetch) {
    pld(mem[r10, 128]); // Prefetch A2
  }

  // BLOCK 3
  vldr(d14, mem[r9, 48]); // B1
  if (max_mr > 3) {
    vmov(d7, r0, r2); // a3 VMOV
  }
  if (max_mr > 1) {
    vmla_f32(q10, q6, d1[1]);
  }
  ldr(r0, mem[r9, 88]); // B0 low
  if (max_mr > 2) {
    vmla_f32(q12, q6, d2[1]);
  }
  ldr(r2, mem[r9, 92]); // B0 high
  if (max_mr > 3) {
    vmla_f32(q14, q6, d3[1]);
  }
  if (prefetch) {
    pld(mem[r7, 128]); // Prefetch A3
  }

  // BLOCK 4
  vldr(d8, mem[r9, 64]); // B0
  vmov(d11, r0, r2); // B0 VMOV
  vmla_f32(q9, q7, d0[1]);
  ldr(r0, mem[r9, 104]); // B1 low   VMOV is in BLOCK 0
  if (max_mr > 1) {
    vmla_f32(q11, q7, d1[1]);
  }
  ldr(r2, mem[r9, 108]); // B1 high
  if (max_mr > 2) {
    vmla_f32(q13, q7, d2[1]);
  }
  if (prefetch) {
    pld(mem[r9, 384]); // Prefetch B
  }

  // BLOCK 5
  vldr(d10, mem[r9, 80]); // B0
  vmov(d13, r0, r2); // b1 VMOV b from second group
  if (max_mr > 3) {
    vmla_f32(q15, q7, d3[1]);
  }
  ldr(r0, mem[r9, 120]); // B1 low   VMOV is in BLOCK 0
  nop();
  ldr(r2, mem[r9, 124]); // B1 high
  nop();
  if (prefetch) {
    pld(mem[r9, 448]); // Prefetch B
  }

  // Second group of 16 FMA, First group of loads
  // BLOCK 0
  vld1_32({d0}, mem[r3]++); // A0
  vmov(d15, r0, r2); // b1 VMOV b from second group
  vmla_f32(q8, q4, d4[0]);
  if (max_mr > 1) {
    ldr(r0, mem[r12, 8]); // A1 low
    vmla_f32(q10, q4, d5[0]);
    ldr(r2, mem[r12, 12]); // A1 high
  }
  if (max_mr > 2) {
    vmla_f32(q12, q4, d6[0]);
  }
  // NOP

  // BLOCK 1
  vldr(d12, mem[r9, 96]); // B1
  if (max_mr > 1) {
    vmov(d1, r0, r2); // a1 VMOV
  }
  if (max_mr > 3) {
    vmla_f32(q14, q4, d7[0]);
  }
  ldr(r0, mem[r9, 136]); // B0 low
  vmla_f32(q9, q5, d4[0]);
  ldr(r2, mem[r9, 140]); // B0 high
  if (max_mr > 1) {
    vmla_f32(q11, q5, d5[0]);
  }
  // NOP

  // BLOCK 2
  if (max_mr > 2) {
    vld1_32({d2}, mem[r10]++); // A2
  }
  vmov(d9, r0, r2); // b0 VMOV
  if (max_mr > 2) {
    vmla_f32(q13, q5, d6[0]);
  }
  if (max_mr > 3) {
    ldr(r0, mem[r7, 8]); // A3 low
    vmla_f32(q15, q5, d7[0]);
    ldr(r2, mem[r7, 12]); // A3 high
  }
  vmla_f32(q8, q6, d4[1]);
  // NOP

  // BLOCK 3
  vldr(d14, mem[r9, 112]); // B1
  if (max_mr > 3) {
    vmov(d3, r0, r2); // a3 VMOV
  }
  if (max_mr > 1) {
    vmla_f32(q10, q6, d5[1]);
  }
  ldr(r0, mem[r9, 152]); // B0 low
  if (max_mr > 2) {
    vmla_f32(q12, q6, d6[1]);
  }
  ldr(r2, mem[r9, 156]); // B0 high
  if (max_mr > 3) {
    vmla_f32(q14, q6, d7[1]);
  }
  if (max_mr > 1) {
    add(r12, r12, 16); // A1++
  }

  // BLOCK 4
  vldr(d8, mem[r9, 128]); // B0
  vmov(d11, r0, r2); // B0 VMOV
  vmla_f32(q9, q7, d4[1]);
  ldr(r0, mem[r9, 168]); // B1 low
  if (max_mr > 1) {
    vmla_f32(q11, q7, d5[1]);
  }
  ldr(r2, mem[r9, 172]); // B1 high
  if (max_mr > 2) {
    vmla_f32(q13, q7, d6[1]);
  }
  if (max_mr > 3) {
    add(r7, r7, 16); // A3++
  }

  // BLOCK 5
  vldr(d10, mem[r9, 144]); // B0
  vmov(d13, r0, r2); // b1 VMOV b
  if (max_mr > 3) {
    vmla_f32(q15, q7, d7[1]);
  }
  ldr(r0, mem[r9, 184]); // B1 low   VMOV is in BLOCK 0
  subs(r5, r5, 16);
  ldr(r2, mem[r9, 188]); // B1 high
  add(r9, r9, 128); // B++
  bhs(l2);

  // Epilogue - 4 floats of A (16 bytes)
  bind(l3);
  // First group of 16 FMA, Second group loads
  // BLOCK 0
  vld1_32({d4}, mem[r3]++); // A0
  vmov(d15, r0, r2); // b1 VMOV b from second group
  vmla_f32(q8, q4, d0[0]);
  if (max_mr > 1) {
    ldr(r0, mem[r12]); // A1 low
    vmla_f32(q10, q4, d1[0]);
    ldr(r2, mem[r12, 4]); // A1 high
  }
  if (max_mr > 2) {
    vmla_f32(q12, q4, d2[0]);
  }
  // NOP

  // BLOCK 1
  vldr(d12, mem[r9, 32]); // B1
  if (max_mr > 1) {
    vmov(d5, r0, r2); // a1 VMOV
  }
  if (max_mr > 3) {
    vmla_f32(q14, q4, d3[0]);
  }
  ldr(r0, mem[r9, 72]); // B0 low
  vmla_f32(q9, q5, d0[0]);
  ldr(r2, mem[r9, 76]); // B0 high
  if (max_mr > 1) {
    vmla_f32(q11, q5, d1[0]);
  }
  // NOP

  // BLOCK 2
  if (max_mr > 2) {
    vld1_32({d6}, mem[r10]++); // A2
  }
  vmov(d9, r0, r2); // b0 VMOV
  if (max_mr > 2) {
    vmla_f32(q13, q5, d2[0]);
  }
  if (max_mr > 3) {
    ldr(r0, mem[r7]); // A3 low
    vmla_f32(q15, q5, d3[0]);
    ldr(r2, mem[r7, 4]); // A3 high
  }
  vmla_f32(q8, q6, d0[1]);
  // NOP

  // BLOCK 3
  vldr(d14, mem[r9, 48]); // B1
  if (max_mr > 3) {
    vmov(d7, r0, r2); // a3 VMOV
  }
  if (max_mr > 1) {
    vmla_f32(q10, q6, d1[1]);
  }
  ldr(r0, mem[r9, 88]); // B0 low
  if (max_mr > 2) {
    vmla_f32(q12, q6, d2[1]);
  }
  ldr(r2, mem[r9, 92]); // B0 high
  if (max_mr > 3) {
    vmla_f32(q14, q6, d3[1]);
  }
  // NOP

  // BLOCK 4
  vldr(d8, mem[r9, 64]); // B0
  vmov(d11, r0, r2); // B0 VMOV
  vmla_f32(q9, q7, d0[1]);
  ldr(r0, mem[r9, 104]); // B1 low
  if (max_mr > 1) {
    vmla_f32(q11, q7, d1[1]);
  }
  ldr(r2, mem[r9, 108]); // B1 high
  if (max_mr > 2) {
    vmla_f32(q13, q7, d2[1]);
  }
  // NOP

  // BLOCK 5
  vldr(d10, mem[r9, 80]); // B0
  vmov(d13, r0, r2); // b1 VMOV b
  if (max_mr > 3) {
    vmla_f32(q15, q7, d3[1]);
  }
  ldr(r0, mem[r9, 120]); // B1 low   VMOV is in BLOCK 0
  nop();
  ldr(r2, mem[r9, 124]); // B1 high
  nop();
  nop();

  // Second group of 16 FMA, First group of loads
  // BLOCK 0
  vldr(d12, mem[r9, 96]); // B1
  vmov(d15, r0, r2); // b1 VMOV b from second group
  vmla_f32(q8, q4, d4[0]);
  if (max_mr > 1) {
    vmla_f32(q10, q4, d5[0]);
  }
  if (max_mr > 2) {
    vmla_f32(q12, q4, d6[0]);
  }

  // BLOCK 1
  vldr(d14, mem[r9, 112]); // B1
  if (max_mr > 3) {
    vmla_f32(q14, q4, d7[0]);
  }
  vmla_f32(q9, q5, d4[0]);
  if (max_mr > 1) {
    vmla_f32(q11, q5, d5[0]);
    add(r12, r12, 8); // A1++
  }

  // BLOCK 2
  if (max_mr > 3) {
    add(r7, r7, 8); // A3++ VLDR B1 lands here
  }
  add(r9, r9, 128); // B++
  if (max_mr > 2) {
    vmla_f32(q13, q5, d6[0]);
  }
  if (max_mr > 3) {
    vmla_f32(q15, q5, d7[0]);
  }
  vmla_f32(q8, q6, d4[1]);

  // BLOCK 3
  if (max_mr > 1) {
    vmla_f32(q10, q6, d5[1]);
  }
  if (max_mr > 2) {
    vmla_f32(q12, q6, d6[1]);
  }
  if (max_mr > 3) {
    vmla_f32(q14, q6, d7[1]);
  }
  tst(r5, 15);

  // BLOCK 4
  vmla_f32(q9, q7, d4[1]);
  if (max_mr > 1) {
    vmla_f32(q11, q7, d5[1]);
  }
  if (max_mr > 2) {
    vmla_f32(q13, q7, d6[1]);
  }

  // BLOCK 5
  if (max_mr > 3) {
    vmla_f32(q15, q7, d7[1]);
  }

  // Is there a remainder?- 1 to 3 floats of A (4, 8 or 12 bytes)
  bne(l5);

  align(8);
  bind(l4);
  ldr(r5, mem[sp, 112]); // a
  subs(r14, r14, max_mr * sizeof(void*)); // ks -= MR * sizeof(void*)

  // ks loop
  bhi(l1);

  // Load params pointer
  ldr(r0, mem[sp, 128]); // cn_stride
  ldr(r2, mem[sp, 140]); // params
  ldr(r14, mem[sp, 72]); // p = ks
  subs(r1, r1, 8);

  // Load min/max values
  if (clamp_min || clamp_max) {
    vld1r_32({d4,d5}, mem[r2]++);
    vld1r_32({d6,d7}, mem[r2]);
  }

  // Clamp
  if (clamp_min) {
    vmax_f32(q8, q8, q2);
    vmax_f32(q9, q9, q2);
    if (max_mr > 1) {
      vmax_f32(q10, q10, q2);
      vmax_f32(q11, q11, q2);
    }
    if (max_mr > 2) {
      vmax_f32(q12, q12, q2);
      vmax_f32(q13, q13, q2);
    }
    if (max_mr > 3) {
      vmax_f32(q14, q14, q2);
      vmax_f32(q15, q15, q2);
    }
  }
  if (clamp_max) {
    vmin_f32(q8, q8, q3);
    vmin_f32(q9, q9, q3);
    if (max_mr > 1) {
      vmin_f32(q10, q10, q3);
      vmin_f32(q11, q11, q3);
    }
    if (max_mr > 2) {
      vmin_f32(q12, q12, q3);
      vmin_f32(q13, q13, q3);
    }
    if (max_mr > 3) {
      vmin_f32(q14, q14, q3);
      vmin_f32(q15, q15, q3);
    }
  }
  perform_post_operations(max_mr, num_post_operations, post_operations);

  // Store full 4 x 8
  blo(l7);
  if (max_mr > 3) {
    vst1_32({d28-d31}, mem[r6], r0);
  }
  if (max_mr > 2) {
    vst1_32({d24-d27}, mem[r8], r0);
  }
  if (max_mr > 1) {
    vst1_32({d20-d23}, mem[r4], r0);
  }
  vst1_32({d16-d19}, mem[r11], r0);

  sub(r5, r5, r14); // a -= ks

  bhi(l0);

  vpop({d8-d15});
  add(sp, sp, 12); // skip pad, r2, r3
  pop({r4, r5, r6, r7, r8, r9, r10, r11, pc});

  align(8);
  bind(l5);
  // Is there a remainder?- 2 floats of A (8 bytes)
  tst(r5, 8);
  beq(l6);

  // Remainder - 2 floats of A (8 bytes)
  vld1_32({d0}, mem[r3]++); // A0
  vldm(mem[r9]++, {d8-d11}); // B0
  if (max_mr > 1) {
    vld1_32({d1}, mem[r12]++); // A1
  }
  if (max_mr > 2) {
    vld1_32({d2}, mem[r10]++); // A2
  }
  if (max_mr > 3) {
    vld1_32({d3}, mem[r7]++); // A3
  }

  vmla_f32(q8, q4, d0[0]);
  vmla_f32(q9, q5, d0[0]);
  if (max_mr > 1) {
    vmla_f32(q10, q4, d1[0]);
    vmla_f32(q11, q5, d1[0]);
  }
  vldm(mem[r9]++, {d12-d15}); // B1
  if (max_mr > 2) {
    vmla_f32(q12, q4, d2[0]);
    vmla_f32(q13, q5, d2[0]);
  }
  if (max_mr > 3) {
    vmla_f32(q14, q4, d3[0]);
    vmla_f32(q15, q5, d3[0]);
  }
  vmla_f32(q8, q6, d0[1]);
  vmla_f32(q9, q7, d0[1]);
  if (max_mr > 1) {
    vmla_f32(q10, q6, d1[1]);
    vmla_f32(q11, q7, d1[1]);
  }
  if (max_mr > 2) {
    vmla_f32(q12, q6, d2[1]);
    vmla_f32(q13, q7, d2[1]);
  }
  if (max_mr > 3) {
    vmla_f32(q14, q6, d3[1]);
    vmla_f32(q15, q7, d3[1]);
  }

  // Is there a remainder?- 1 float of A (4 bytes)
  tst(r5, 4);
  beq(l4);

  bind(l6);
  // Remainder- 1 float of A (4 bytes)
  vldm(mem[r3]++, {s0}); // A0
  vldm(mem[r9]++, {d8-d11}); // B0
  if (max_mr > 1) {
    vldm(mem[r12]++, {s2}); // A1
  }
  if (max_mr > 2) {
    vldm(mem[r10]++, {s4}); // A2
  }
  if (max_mr > 3) {
    vldm(mem[r7]++, {s6}); // A3
  }
  vmla_f32(q8, q4, d0[0]);
  vmla_f32(q9, q5, d0[0]);
  if (max_mr > 1) {
    vmla_f32(q10, q4, d1[0]);
    vmla_f32(q11, q5, d1[0]);
  }
  if (max_mr > 2) {
    vmla_f32(q12, q4, d2[0]);
    vmla_f32(q13, q5, d2[0]);
  }
  if (max_mr > 3) {
    vmla_f32(q14, q4, d3[0]);
    vmla_f32(q15, q5, d3[0]);
  }
  b(l4);

  // Store odd width
  bind(l7);
  tst(r1, 4);
  beq(l8);
  if (max_mr > 3) {
    vst1_32({d28-d29}, mem[r6]++);
  }
  if (max_mr > 2) {
    vst1_32({d24-d25}, mem[r8]++);
  }
  if (max_mr > 3) {
    vmov(q14, q15);
  }
  if (max_mr > 2) {
    vmov(q12, q13);
  }
  if (max_mr > 1) {
    vst1_32({d20-d21}, mem[r4]++);
  }
  vst1_32({d16-d17}, mem[r11]++);
  if (max_mr > 1) {
    vmov(q10, q11);
  }
  vmov(q8, q9);

  bind(l8);
  tst(r1, 2);
  beq(l9);
  if (max_mr > 3) {
    vst1_32({d28}, mem[r6]++);
  }
  if (max_mr > 2) {
    vst1_32({d24}, mem[r8]++);
  }
  if (max_mr > 3) {
    vmov(d28, d29);
  }
  if (max_mr > 2) {
    vmov(d24, d25);
  }
  if (max_mr > 1) {
    vst1_32({d20}, mem[r4]++);
  }
  vst1_32({d16}, mem[r11]++);
  if (max_mr > 1) {
    vmov(d20, d21);
  }
  vmov(d16, d17);

  bind(l9);
  tst(r1, 1);
  beq(l10);
  if (max_mr > 3) {
    vst1_32({d28[0]}, mem[r6]++);
  }
  if (max_mr > 2) {
    vst1_32({d24[0]}, mem[r8]++);
  }
  if (max_mr > 1) {
    vst1_32({d20[0]}, mem[r4]++);
  }
  vst1_32({d16[0]}, mem[r11]++);

  bind(l10);
  vpop({d8-d15});
  add(sp, sp, 12); // skip pad, r2, r3
  pop({r4, r5, r6, r7, r8, r9, r10, r11, pc});

  align(16);
}

void Generator::perform_post_operations(
  size_t max_mr,
  size_t num_post_operations,
  const xnn_post_operation* post_operations)
{
  if (num_post_operations == 0) {
    return;
  }
  for (size_t i = 0; i < num_post_operations; i++) {
    switch (post_operations[i].op_type) {
      case xnn_post_operation_type_hardswish: {
        const auto sixth = q0;
        const auto three = q1;
        const auto six = q2;
        const auto zero = q3;
        vld3r_32({sixth.low(), three.low(), six.low()}, mem[r2]++);
        vmov(zero, 0);
        vmov(three.high(), three.low());
        vmov(six.high(), six.low());
        const QRegister accs[] = {q8, q9, q10, q11, q12, q13, q14, q15};
        const QRegister tmps[] = {q4, q5, q6, q7};
        f32_hardswish(sixth, three, six, zero, &accs[0], XNN_COUNT_OF(accs), &tmps[0], XNN_COUNT_OF(tmps));
        break;
      }
      default:
        XNN_UNREACHABLE;
    }
  }
}

}  // namespace
}  // namespace aarch32
}  // namespace xnnpack

xnn_status_t xnn_generate_f32_igemm_ukernel_4x8__aarch32_neon_cortex_a53(xnn_code_buffer* code, size_t max_mr, size_t nc_mod_nr, size_t kc, size_t ks, const void* params) {
  using namespace xnnpack::aarch32;
  Generator g(code);
  assert(params != nullptr);
  g.generate(false, max_mr, nc_mod_nr, kc, ks, static_cast<const jit_gemm_params*>(params));
  g.finalize();
  if (g.error() != xnnpack::Error::kNoError) {
    return xnn_status_invalid_state;
  }
  return xnn_status_success;
}

xnn_status_t xnn_generate_f32_igemm_ukernel_4x8__aarch32_neon_prfm_cortex_a53(xnn_code_buffer* code, size_t max_mr, size_t nc_mod_nr, size_t kc, size_t ks, const void* params) {
  using namespace xnnpack::aarch32;
  Generator g(code);
  assert(params != nullptr);
  g.generate(true, max_mr, nc_mod_nr, kc, ks, static_cast<const jit_gemm_params*>(params));
  g.finalize();
  if (g.error() != xnnpack::Error::kNoError) {
    return xnn_status_invalid_state;
  }
  return xnn_status_success;
}
