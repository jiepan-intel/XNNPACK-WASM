
curr=$(pwd)
for  d8 in d8-org d8
do
    echo ${d8}
    d8bin=/home/panjie/code/backup/web/v82/v8/out.gn/x64.release/${d8}
    for dir in bench-org #bench-i32-to-f32
    do
        cd ${dir}
        for bench in end2end_bench.js  f32_vhswish_bench.js # f32_gemm_bench.js f32_spmm_bench.js
        do
            echo -e "\n"
            ${d8bin}  --wasm-slp-vectorize  ${bench}
            echo -e "\n"
        done
        cd ${curr}
    done
done
