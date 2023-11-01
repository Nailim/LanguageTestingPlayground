.PHONY: help
.PHONY: version
.PHONY: clean
.PHONY: build
.PHONY: analyze

help:
	@echo Help ...
	@echo Run \"make clean\" to clean
	@echo Run \"make build\" to build
	@echo Run \"make analyze\" to print out obj sizes

version:
	@echo
	gcc --version
	
	@echo
	clang --version
	
	@echo
	zig version
	
	@echo
	rustc --version
	
	@echo
	go version

clean: clean-gcc clean-clang clean-zig clean-rust clean-go clean-odin
build: build-gcc build-clang build-zig build-rust build-go build-odin

analyze: analyze-gcc analyze-clang analyze-zig analyze-rust analyze-go analyze-odin
	# ls -l



build-gcc: clean-gcc compile-gcc

compile-gcc: hello_world.c hello_world_syscall.c
	gcc -static hello_world.c -o gcc_hello_world
	gcc -static hello_world_syscall.c -o gcc_hello_world_syscall
	
	gcc -static -s hello_world.c -o gcc_stripped_hello_world
	gcc -static -s hello_world_syscall.c -o gcc_stripped_hello_world_syscall
	
	gcc -O3 -static -s hello_world.c -o gcc_stripped_03_hello_world
	gcc -O3 -static -s hello_world_syscall.c -o gcc_stripped_03_hello_world_syscall
	
	gcc -Os -static -s hello_world.c -o gcc_stripped_0s_hello_world
	gcc -Os -static -s hello_world_syscall.c -o gcc_stripped_0s_hello_world_syscall

clean-gcc:
	rm -f gcc_*

analyze-gcc:
	@size gcc_hello_world gcc_hello_world_syscall \
	gcc_stripped_hello_world gcc_stripped_hello_world_syscall \
	gcc_stripped_03_hello_world gcc_stripped_03_hello_world_syscall \
	gcc_stripped_0s_hello_world gcc_stripped_0s_hello_world_syscall



build-clang: clean-clang compile-clang

compile-clang: hello_world.c hello_world_syscall.c
	clang -static hello_world.c -o clang_hello_world
	clang -static hello_world_syscall.c -o clang_hello_world_syscall
	
	clang -static -s hello_world.c -o clang_stripped_hello_world
	clang -static -s hello_world_syscall.c -o clang_stripped_hello_world_syscall
	
	clang -O3 -static -s hello_world.c -o clang_stripped_03_hello_world
	clang -O3 -static -s hello_world_syscall.c -o clang_stripped_03_hello_world_syscall
	
	clang -Os -static -s hello_world.c -o clang_stripped_0s_hello_world
	clang -Os -static -s hello_world_syscall.c -o clang_stripped_0s_hello_world_syscall

clean-clang:
	rm -f clang_*

analyze-clang:
	@size clang_hello_world clang_hello_world_syscall \
	clang_stripped_hello_world clang_stripped_hello_world_syscall \
	clang_stripped_03_hello_world clang_stripped_03_hello_world_syscall \
	clang_stripped_0s_hello_world clang_stripped_0s_hello_world_syscall



build-zig: clean-zig compile-zig

compile-zig:
	zig build-exe hello_world.zig --name zig_hello_world
	zig build-exe hello_world_syscall.zig --name zig_hello_world_syscall

	zig build-exe -fstrip hello_world.zig --name zig_stripped_hello_world
	zig build-exe -fstrip hello_world_syscall.zig --name zig_stripped_hello_world_syscall

	zig build-exe -OReleaseFast -fstrip hello_world.zig --name zig_stripped_03_hello_world
	zig build-exe -OReleaseFast -fstrip hello_world_syscall.zig --name zig_stripped_03_hello_world_syscall
	
	zig build-exe -OReleaseSmall -fstrip hello_world.zig --name zig_stripped_0s_hello_world
	zig build-exe -OReleaseSmall -fstrip hello_world_syscall.zig --name zig_stripped_0s_hello_world_syscall


clean-zig:
	rm -f zig_*

analyze-zig:
	@size zig_hello_world zig_hello_world_syscall \
	zig_stripped_hello_world zig_stripped_hello_world_syscall \
	zig_stripped_03_hello_world zig_stripped_03_hello_world_syscall \
	zig_stripped_0s_hello_world zig_stripped_0s_hello_world_syscall



build-rust: clean-rust compile-rust

compile-rust:
	# rust build system is _great_ https://msfjarvis.dev/posts/building-static-rust-binaries-for-linux/

	rustc -C target-feature=+crt-static hello_world.rs -o rust_hello_world
	cd hello_world_syscall_rust && RUSTFLAGS='-C target-feature=+crt-static' cargo build --release --target x86_64-unknown-linux-gnu && cp target/x86_64-unknown-linux-gnu/release/hello_world_syscall_rust ../rust_hello_world_syscall
	
	rustc -C strip=symbols -C target-feature=+crt-static hello_world.rs -o rust_stripped_hello_world
	cd hello_world_syscall_rust && RUSTFLAGS='-C strip=symbols -C target-feature=+crt-static' cargo build --release --target x86_64-unknown-linux-gnu && cp target/x86_64-unknown-linux-gnu/release/hello_world_syscall_rust ../rust_stripped_hello_world_syscall

	rustc -C strip=symbols -C opt-level=z -C target-feature=+crt-static hello_world.rs -o rust_stripped_size_hello_world
	cd hello_world_syscall_rust && RUSTFLAGS='-C strip=symbols -C opt-level=z -C target-feature=+crt-static' cargo build --release --target x86_64-unknown-linux-gnu && cp target/x86_64-unknown-linux-gnu/release/hello_world_syscall_rust ../rust_stripped_size_hello_world_syscall

clean-rust:
	rm -f rust_*
	cd hello_world_syscall_rust && cargo clean

analyze-rust:
	@size rust_hello_world rust_hello_world_syscall \
	rust_stripped_hello_world rust_stripped_hello_world_syscall \
	rust_stripped_size_hello_world rust_stripped_size_hello_world_syscall



build-go: clean-go compile-go

compile-go:
	go build -o go_hello_world -ldflags="-extldflags=-static" hello_world.go
	go build -o go_hello_world_syscall -ldflags="-extldflags=-static" hello_world_syscall.go

	go build -o go_stripped_hello_world -ldflags="-extldflags=-static -s -w" hello_world.go
	go build -o go_stripped_hello_world_syscall -ldflags="-extldflags=-static -s -w" hello_world_syscall.go
	
clean-go:
	rm -f go_*

analyze-go:
	@size go_hello_world go_hello_world_syscall \
	go_stripped_hello_world go_stripped_hello_world_syscall



build-odin: clean-odin compile-odin

compile-odin:
	odin build hello_world.odin -file -out:odin_hello_world
	strip -o odin_stripped_hello_world odin_hello_world
	
	odin build hello_world_syscall.odin -file -out:odin_hello_world_syscall
	strip -o odin_stripped_hello_world_syscall odin_hello_world_syscall
	
	odin build hello_world.odin -file -o:speed -out:odin_speed_hello_world
	strip -o odin_stripped_speed_hello_world odin_hello_world
	
	odin build hello_world_syscall.odin -file -o:speed -out:odin_speed_hello_world_syscall
	strip -o odin_stripped_speed_hello_world_syscall odin_hello_world_syscall
	
	odin build hello_world.odin -file -o:size -out:odin_size_hello_world
	strip -o odin_stripped_size_hello_world odin_hello_world
	
	odin build hello_world_syscall.odin -file -o:size -out:odin_size_hello_world_syscall
	strip -o odin_stripped_size_hello_world_syscall odin_hello_world_syscall
	
clean-odin:
	rm -f odin_*

analyze-odin:
	@size odin_hello_world odin_hello_world_syscall \
	odin_stripped_hello_world odin_stripped_hello_world_syscall \
	odin_stripped_speed_hello_world odin_stripped_speed_hello_world_syscall \
	odin_stripped_size_hello_world odin_stripped_size_hello_world_syscall
