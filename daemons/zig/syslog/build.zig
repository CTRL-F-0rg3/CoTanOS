const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{
        .default_target = .{
            .cpu_arch = .riscv64,
            .os_tag = .freestanding,
        },
    });

    const exe = b.addExecutable(.{
        .name = "syslog",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = .ReleaseSmall,
    });

    exe.linkage = .static;
    exe.entry = .disabled;
    exe.setLinkerScriptPath(.{ .path = "../../link.ld" });

    b.installArtifact(exe);
}
