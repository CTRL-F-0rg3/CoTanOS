qemu-system-riscv64 \
  -machine virt \
  -cpu rv64 \
  -smp 4 \
  -m 2G \
  -bios default \
  -kernel target/riscv64gc-unknown-none-elf/debug/twój_kernel.elf \
  -nographic \
  -serial mon:stdio
