CXX = aarch64-linux-android24-clang++
OBJDUMP = llvm-objdump
output_dir = neon_test

all: lib rgb matmul collision collision_deinterleave

lib: 
	@mkdir -p ${output_dir} 

rgb:
	${CXX} -g -o3 rgb.cpp -o ./${output_dir}/rgb

matmul:
	${CXX} -g -o3 matmul.cpp -o ./${output_dir}/matmul

collision:
	${CXX} -S -o3 collision.cpp -o ./${output_dir}/collision.s
	${CXX} -g -o3 collision.cpp -o ./${output_dir}/collision

collision_deinterleave:
	${CXX} -g -o3 collision_deinterleave.cpp -o ./${output_dir}/collision_deinterleave

clean:
	rm -f ${output_dir}/*
