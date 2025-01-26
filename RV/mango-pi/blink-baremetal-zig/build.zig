const std = @import("std");
const CrossTarget = @import("std").zig.CrossTarget;
const Target = @import("std").Target;
const Feature = @import("std").Target.Cpu.Feature;

pub fn build(b: *std.Build) void {
    // const features = Target.riscv.Feature;

    const disabled_features = Feature.Set.empty;
    const enabled_features = Feature.Set.empty;

    // var disabled_features = Feature.Set.empty;
    // var enabled_features = Feature.Set.empty;

    // // disable all CPU extensions
    // disabled_features.addFeature(@intFromEnum(features.a));
    // disabled_features.addFeature(@intFromEnum(features.c));
    // disabled_features.addFeature(@intFromEnum(features.d));
    // disabled_features.addFeature(@intFromEnum(features.e));
    // disabled_features.addFeature(@intFromEnum(features.f));
    // // except multiply
    // enabled_features.addFeature(@intFromEnum(features.m));

    const target = b.resolveTargetQuery(.{ .cpu_arch = Target.Cpu.Arch.riscv64, .os_tag = Target.Os.Tag.freestanding, .abi = Target.Abi.none, .cpu_model = .{ .explicit = &std.Target.riscv.cpu.generic_rv64 }, .cpu_features_sub = disabled_features, .cpu_features_add = enabled_features });

    const exe = b.addExecutable(.{
        .name = "blink",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = .ReleaseSmall,
    });

    exe.setLinkerScript(b.path("src/memmap.ld"));

    exe.addAssemblyFile(b.path("src/start.S"));

    b.installArtifact(exe);

    const bin = b.addObjCopy(exe.getEmittedBin(), .{
        .format = .bin,
    });
    bin.step.dependOn(&exe.step);

    const copy_bin = b.addInstallBinFile(bin.getOutput(), "blink.bin");
    b.default_step.dependOn(&copy_bin.step);

    const hex = b.addObjCopy(exe.getEmittedBin(), .{ .format = .hex });
    hex.step.dependOn(&exe.step);

    const copy_hex = b.addInstallBinFile(hex.getOutput(), "blink.hex");
    b.default_step.dependOn(&copy_hex.step);

    const objcopy_srec_cmd = b.addSystemCommand(&.{ "objcopy", "--srec-forceS3", "blink", "-O", "srec", "blink.srec" });
    objcopy_srec_cmd.step.dependOn(&copy_bin.step);
    const run_step = b.step("run", "Generate .step file");
    run_step.dependOn(&objcopy_srec_cmd.step);
}
