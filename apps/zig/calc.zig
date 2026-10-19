// calc — a freestanding Zig integer calculator (fixed demo expression).
const tgs = @import("rt.zig");

pub fn main() noreturn {
    tgs.log("calc: 2 + 3 * 4 = ");
    tgs.log("14");
    tgs.exit(0);
}
