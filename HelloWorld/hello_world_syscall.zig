const std = @import("std");
const linux = std.os.linux;

pub export fn _start() noreturn {
    _ = linux.write(1, "Hello, World!\n", 14);
    linux.exit(0);
}
