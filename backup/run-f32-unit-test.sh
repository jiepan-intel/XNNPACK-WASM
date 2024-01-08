
#for testcase in f16_f32_vcvt_test f32_argmaxpool_test f32_avgpool_minmax_test f32_ibilinear_test f32_ibilinear_chw_test f32_igemm_test f32_igemm_relu_test f32_igemm_minmax_test f32_conv_hwc_test f32_conv_hwc2chw_test f32_dwconv_test f32_dwconv_minmax_test f32_dwconv2d_chw_test f32_f16_vcvt_test f32_gavgpool_minmax_test f32_gavgpool_cw_test f32_gemm_test f32_gemm_relu_test f32_gemm_minmax_test f32_gemminc_minmax_test f32_vhswish_test f32_maxpool_minmax_test f32_pavgpool_minmax_test f32_ppmm_minmax_test f32_prelu_test f32_qs8_vcvt_test f32_qu8_vcvt_test f32_raddexpminusmax_test f32_raddextexp_test f32_raddstoreexpminusmax_test f32_rmax_test f32_spmm_minmax_test f32_vabs_test f32_vadd_test f32_vadd_minmax_test f32_vadd_relu_test f32_vaddc_test f32_vaddc_minmax_test f32_vaddc_relu_test f32_vclamp_test f32_vdiv_test f32_vdiv_minmax_test f32_vdiv_relu_test f32_vdivc_test f32_vdivc_minmax_test f32_vdivc_relu_test f32_vrdivc_test f32_vrdivc_minmax_test f32_vrdivc_relu_test f32_velu_test f32_vmax_test f32_vmaxc_test f32_vmin_test f32_vminc_test f32_vmul_test f32_vmul_minmax_test f32_vmul_relu_test f32_vmulc_test f32_vmulc_minmax_test f32_vmulc_relu_test f32_vmulcaddc_minmax_test f32_vlrelu_test f32_vneg_test f32_vrelu_test f32_vrndne_test f32_vrndz_test f32_vrndu_test f32_vrndd_test f32_vscaleexpminusmax_test f32_vscaleextexp_test f32_vsigmoid_test f32_vsqr_test f32_vsqrdiff_test f32_vsqrdiffc_test f32_vsqrt_test f32_vsub_test f32_vsub_minmax_test f32_vsub_relu_test f32_vsubc_test f32_vsubc_minmax_test f32_vsubc_relu_test f32_vrsubc_test f32_vrsubc_minmax_test f32_vrsubc_relu_test qs8_f32_vcvt_test qu8_f32_vcvt_test

#for testcase in f32_spmm_minmax_test 
#for testcase in f32_f16_vcvt_test

for testcase in f32_qs8_vcvt_test f32_qu8_vcvt_test f32_vhswish_test
do
#--trace-wasm-revectorize  
#/home/panjie/apx/work/v8-revec/v8/out.gn/x64.release/d8  --turboshaft-wasm  --turboshaft-wasm-instruction-selection-experimental --turboshaft-instruction-selection --turboshaft-wasm-instruction-selection-staged   --no-liftoff --no-wasm_lazy_compilation --experimental-wasm-revectorize  ${testcase}.js 
/home/panjie/apx/work/v8-revec/v8/out.gn/x64.debug/d8  --turboshaft-wasm  --turboshaft-wasm-instruction-selection-experimental --turboshaft-instruction-selection --turboshaft-wasm-instruction-selection-staged   --no-liftoff --no-wasm_lazy_compilation --experimental-wasm-revectorize  ${testcase}.js 
done


