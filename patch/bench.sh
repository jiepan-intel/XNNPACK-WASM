FILTER=" --  --benchmark_filter=FP32"
D8="/home/panjie/web/src/v82/v8/out.gn/x64.release/d8"

${D8}   --wasm-slp-vectorize    ./end2end_bench.js  ${FILTER}
${D8}                           ./end2end_bench.js  ${FILTER}
