
bench="f32_gemm_bench"
#for branch in 4x16baseline  4x16aligned  4x16hack256
#for branch in avx-base  avx-loadsplat avx-loadsplat-aligned  avx-loadsplat-aligned-hack256
#for branch in  scalable_base  scalable_loadsplat  scalable_4xN_8 scalable_4xN_16
#for branch in    scalable_4xN_8 scalable_4xN_16

#scalable_base scalable_opt 

  #for branch in  scalable_base scalable_leftover_opt
for branch in  scalable_base #scalable_dwconv_opt scalable_relu_opt
do
    git checkout ${branch}
    make f32_gemm_bench

    out=wasm-out/${bench}/${branch}
    mkdir -p ${out}

    cp -f bazel-bin/${bench}.js   ${out}
    if [[ "$branch" == *"_opt"* ]]
    then
        cp -f bazel-bin/${bench}.wasm  ${out}/${bench}-org.wasm 
        inject-hint.sh ${out}/${bench}-org.wasm ${out}/${bench}.wasm
    else
        cp -f bazel-bin/${bench}.wasm  ${out}/${bench}.wasm 
    fi


done
