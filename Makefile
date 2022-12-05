VERBOSE=--subcommands  --verbose_explanations --explain log.log
PROXY=--action_env=HTTP_PROXY=$HTTP_PROXY

#WASMSIMD=--cpu=wasm  --copt=-msimd128  --crosstool_top=//toolchain:emscripten --spawn_strategy=local --host_crosstool_top=@bazel_tools//tools/cpp:toolchain

#WASMSIMD=--cpu=wasm  --copt=-msimd128  --crosstool_top=@emsdk//emscripten_toolchain:everything --spawn_strategy=local --host_crosstool_top=@bazel_tools//tools/cpp:toolchain
WASMSIMD=--cpu=wasm  --features=wasm_simd  --crosstool_top=@emsdk//emscripten_toolchain:everything --spawn_strategy=local --host_crosstool_top=@bazel_tools//tools/cpp:toolchain

hashid=2e3bdbf9b98f7f6bc381f42b90e0379b

#WASMSIMD=--config=wasm


D8=/home/panjie/web/src/v82/v8/out.gn/x64.release-vtune/d8

#all: f32_gemm_test
#all: f32_gemm_minmax_test
#all: f32_gemm_bench
#all: f32_gemm_e2e_bench
all: end2end_bench f32_gemm_bench f32_vhswish_bench f32_spmm_bench

f32_gemm_e2e_bench:
	bazel build $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_gemm_e2e_bench 2>&1 | tee log2.log
#mv -f bazel-bin/f32_gemm_e2e_bench bazel-bin/f32_gemm_e2e_bench.js

f32_gemm_bench:
	bazel build $(PROXY) $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_gemm_bench 2>&1 | tee log2.log

f32_spmm_bench:
	bazel build $(PROXY) $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_spmm_bench 2>&1 | tee log2.log

end2end_bench:
	bazel build  $(PROXY) $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) end2end_bench 2>&1 | tee log2.log

f32_gemm_test:
	bazel build  $(PROXY)  $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_gemm_test  2>&1 | tee log2.log

f32_gemm_minmax_test:
	bazel build  $(PROXY)  $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_gemm_minmax_test  2>&1 | tee log2.log

f32_gemm_relu_bench:
	bazel build  $(PROXY)  $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_gemm_relu_bench  2>&1 | tee log2.log

f32_gemm_relu_test:
	bazel build  $(PROXY)  $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_gemm_relu_test  2>&1 | tee log2.log

f32_vhswish_bench:
	bazel build  $(PROXY)  $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_vhswish_bench  2>&1 | tee log2.log

f32_vhswish_test:
	bazel build  $(PROXY)  $(VERBOSE) $(DEFINE) -c opt  $(WASMSIMD) f32_vhswish_test  2>&1 | tee log2.log

clean:
	bazel clean --expunge
	bazel fetch end2end_bench
#cp patch/benchmark.h /home/panjie/.cache/bazel/_bazel_panjie/$(hashid)/external/com_google_benchmark/include/benchmark/benchmark.h
#	cp patch/BUILD.bazel /home/panjie/.cache/bazel/_bazel_panjie/$(hashid)/external/cpuinfo/BUILD.bazel
#
	#symbol -g2
#old
	#cp patch/crosstool.bzl  /home/panjie/.cache/bazel/_bazel_panjie/$(hashid)/external/emsdk/emscripten_toolchain/crosstool.bzl
#new
	cp patch/toolchain.bzl /home/panjie/.cache/bazel/_bazel_panjie/$(hashid)/external/emsdk/emscripten_toolchain/toolchain.bzl


run:
	cd bazel-bin && $(D8) --experimental-wasm-simd  ./f32_gemm_bench.js -- --benchmark_filter=loadsplat_x86/mobilenet_v1
#cd bazel-bin && $(D8) --experimental-wasm-simd  ./f32_gemm_test.js

#external/com_google_googletest/googletest/include/gtest/internal/gtest-internal.h
