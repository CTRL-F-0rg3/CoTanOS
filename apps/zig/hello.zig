// hello — the smallest TrangorgeOS Zig app: print and exit.
const tgs = @import("rt.zig");

pub fn main() noreturn {
    tgs.log("hello from Zig on TrangorgeOS");
    tgs.exit(0);
}
