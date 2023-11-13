const std = @import("std");

pub fn build(b: *std.Build) void {
    const optimize = b.standardOptimizeOption(.{});

    const obj = b.addObject(.{
        .name = "zavr",
        // .root_source_file = .{ .path = "../nano_blink.c" },
        .target = .{
            .cpu_arch = .avr,
            .os_tag = .freestanding,
            .abi = .none,
            .cpu_model = .{ .explicit = &std.Target.avr.cpu.atmega328p },
        },
        .optimize = optimize,
    });
    obj.addCSourceFile(.{ .file = std.build.LazyPath.relative("../nano_blink.c"), .flags = &.{ "-s", "-O3" } });
    obj.addIncludePath(.{ .path = "/usr/avr/include" });
    obj.defineCMacro("F_CPU", "16000000UL");

    const raw_elf = b.addInstallFileWithDir(obj.getEmittedBin(), .prefix, "zig_cc_nano_blink.elf");

    const gcc = b.addSystemCommand(&.{ "avr-gcc", "-mmcu=atmega328p" });
    gcc.addFileArg(obj.getEmittedBin());
    gcc.addArg("-o");
    const gcc_bin = gcc.addOutputFileArg("zig_cc_nano_blink");

    const objcopy = b.addSystemCommand(&.{ "avr-objcopy", "-O", "ihex", "-R", ".eeprom" });
    objcopy.addFileArg(gcc_bin);
    const objcopy_hex = objcopy.addOutputFileArg("zig_cc_nano_blink.hex");

    const install = b.addInstallFileWithDir(objcopy_hex, .prefix, "zig_cc_nano_blink.hex");

    b.getInstallStep().dependOn(&raw_elf.step);
    b.getInstallStep().dependOn(&install.step);
}
