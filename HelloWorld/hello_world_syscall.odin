package main


import "core:sys/unix"

main :: proc() {
	//text :: string("Hello, World!\n")
	unix.sys_write(1, raw_data(string("Hello, World!\n")), 14)
}

