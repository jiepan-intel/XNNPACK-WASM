#! /bin/bash

bench="end2end_bench"
#for branch in avx-base  avx-loadsplat  avx-loadsplat-aligned  avx-loadsplat-aligned-hack256
#for branch in scalable_base  scalable_loadsplat  scalable_8  scalable_16

#for branch in scalable_base scalable_opt
#for branch in scalable_leftover_opt


for branch in  scalable_base  scalable_leftover_opt_new  scalable_dwconv_opt   scalable_relu_opt scalable_relu_min_opt
#  scalable_loadsplat 
#for branch in scalable_base scalable_opt scalable_leftover_opt

#for branch in scalable_leftover_opt
do
    git checkout ${branch}
    git apply ./fixed-iter.patch
    #git apply ./4x8-minmax-to-linear.patch
    #git apply ./v2-model-minmax.patch

    make end2end_bench

    git apply --reverse ./fixed-iter.patch
    #git apply --reverse  ./4x8-minmax-to-linear.patch
    #git apply --reverse ./v2-model-minmax.patch

    out=wasm-out/${bench}/${branch}_fixiter
#out=wasm-out/${bench}/test

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
