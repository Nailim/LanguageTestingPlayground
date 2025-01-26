const std = @import("std");

const GPIO_BASE: u32 = 0x02000000;
const PD_CFG2: u32 = GPIO_BASE + 0x0098;
const PC_DATA: u32 = GPIO_BASE + 0x00A0;

// Function prototypes for PUT32 and GET32
pub extern fn PUT32(address: u32, value: u32) void;
pub extern fn GET32(address: u32) u32;
pub extern fn dummy(value: u32) void;

// pub fn main() !void {
export fn main() noreturn {
    var rx: u32 = 0;

    // Configure PD18 as output
    rx = GET32(PD_CFG2);
    rx &= ~@as(u32, 0x00000F00);
    rx |= 0x00000100;
    PUT32(PD_CFG2, rx);

    const n: u32 = 0x1000000;

    while (true) {
        PUT32(PC_DATA, 0x00040000);
        for (0..n) |i| {
            dummy(@intCast(i));
        }
        PUT32(PC_DATA, 0x00000000);
        for (0..n) |i| {
            dummy(@intCast(i));
        }
    }
}
