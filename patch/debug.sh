#/home/panjie/web/src/v8/v8/out.gn/x64.release-vtune/d8    --experimental-wasm-simd  --wasm-num-compilation-tasks=0 --trace-turbo-graph  --trace-turbo  --trace-turbo-filter="wasm-function#113" f32_gemm_bench.js    --  --benchmark_filter=f32_gemm_4x8__wasmsimd_x86_loadsplat/mobilenet_v1/M:12544/N:32/K:27/real_time --benchmark_repetitions=10
rm *.json

#spmm
#funcid=471
#gemm 4x8splat
#funcid=438
#funcname="wasm-function#${funcid}"

funcname="xnn_f32_gemm_minmax_ukernel_4x8__wasmsimd_x86_splat"

D8="/home/panjie/web/src/v82/v8/out.gn/x64.release/d8"
FILTER=" --  --benchmark_filter=FP32"

#FLAGS=" --no-liftoff  --experimental-wasm-simd    --wasm-num-compilation-tasks=0  --trace-turbo-graph  --trace-turbo  --trace-turbo-filter=${funcname} --wasm-slp-vectorize --trace-wasm-slp --wasm-slp-function-index=${funcid}"
FLAGS=" --no-liftoff  --experimental-wasm-simd    --wasm-num-compilation-tasks=0  --trace-turbo-graph  --trace-turbo  --trace-turbo-filter=${funcname} --wasm-slp-vectorize --trace-wasm-slp"



echo " ${D8} ${FLAGS}  end2end_bench.js   ${FILTER}"
${D8} ${FLAGS}  end2end_bench.js   ${FILTER}  > log.log 2>&1

sed -n "/Begin compiling method ${funcname} /, /Finished compiling method ${funcname} / p" log.log > ${funcname}.txt 2>&1
