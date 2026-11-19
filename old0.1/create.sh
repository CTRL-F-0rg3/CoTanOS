#!/usr/bin/env bash
set -euo pipefail

N=new
K=$N/krn
A=$N/arc
D=$N/drv

mk() { for p in "$@"; do mkdir -p "$(dirname "$p")"; touch "$p"; done; }
dr() { mkdir -p "$@"; }
crate() { local n=$1; shift; mk $K/$n/Cargo.toml; for f in "$@"; do mk $K/$n/src/$f.rs; done; }
zigdrv() { mk $1/{drv.toml,README.md,build.zig,src/main.zig}; }

mkdrv() {
  local p=$1 l=$2 n=$3
  local d=$D/$p
  local id=${p##*/}
  local cls=${p%%/*}
  local lang
  mk $d/drv.toml $d/README.md
  case $l in
    z) mk $d/build.zig $d/src/{main,reg,irq,dma,tst}.zig; lang=zig;;
    r) mk $d/Cargo.toml $d/src/{main,lib,reg,irq,dma}.rs $d/tests/mock.rs; lang=rust;;
    c) mk $d/Makefile $d/include/$id.h $d/src/{main,reg,irq,dma}.c; lang=c;;
    p) mk $d/CMakeLists.txt $d/src/{main,reg,irq,dma}.cpp; lang=cpp;;
    d) mk $d/dub.sdl $d/source/{app,reg,irq,dma}.d; lang=d;;
    o) mk $d/src/{main,reg,irq,dma}.odin; lang=odin;;
  esac
  printf 'name = "%s"\nid = "%s"\nclass = "%s"\nlang = "%s"\nlicense = "GPL-3.0-only"\nstatus = "planned"\narch = ["x86", "rv6"]\ncaps = []\n' "$n" "$id" "$cls" "$lang" > $d/drv.toml
  printf '# %s\n\n%s\n' "$id" "$n" > $d/README.md
}

hdr() { printf '%s SPDX-FileCopyrightText: 2026 CTRL-F-0rg3\n%s SPDX-License-Identifier: %s\n' "$2" "$2" "$3" > "$1"; }

mk $N/{README.md,LICENSE,Cargo.toml,Cargo.x86.toml,Cargo.rv6.toml,rust-toolchain.toml,build.zig,build.zig.zon,.zig-version,Makefile}
mk MIGRATION.md

crate cor lib err addr id bits log sync macros
crate hal lib cpu mmu irq tmr ctx smp io con
crate mem lib frm slb arn zon map stat
crate vmm lib spc reg pgf cow shm tlb
crate sch lib thr prc que swt idl blk bal
crate pol lib trt rrb rtm
crate ipc lib msg chn snd ntf buf tmo
crate cap lib tbl typ rgt der rvk
crate int lib dsp rte exc
crate tmr lib clk tck wht
crate sys lib tbl prc mem ipc cap drv
crate ini lib main cfg pnc usr
crate net lib drv rx tx irq
mk $K/net/LICENSE
mk $K/cc/{Makefile,src/early.c,src/mem.c,src/str.c}

mk $A/x86/{Cargo.toml,link.ld,target.json}
mk $A/x86/asm/{entry,trap,ctx,sysc,smp}.S
mk $A/x86/src/{lib,gdt,idt,tss,apc,ioa,mmu,cpu,tmr,smp,acp,pci,hal_impl,con}.rs
mk $A/x86/bot/limine.conf $A/x86/bot/src/{lib,lmn,uef}.rs
for d in ps2 pic pit ata hpt; do zigdrv $A/x86/drv/$d; done

mk $A/rv6/{Cargo.toml,link.ld,target.json}
mk $A/rv6/asm/{entry,trap,ctx,smp}.S
mk $A/rv6/src/{lib,csr,trp,mmu,plc,clt,sbi,dtb,pmp,smp,hal_impl,con}.rs
mk $A/rv6/bot/src/{lib,sbi,dtb}.rs
for d in sbi clt plc; do zigdrv $A/rv6/drv/$d; done

mk $D/README.md

while read -r p l n; do mkdrv "$p" "$l" "$n"; done <<'EOF'
sto/nvm z NVM Express controller
sto/ahc z AHCI SATA controller
sto/sdc r SD/SDIO host controller
sto/emm z eMMC host controller
sto/ufs z UFS host controller
sto/vbk z virtio block device
sto/rdr z PCIe card reader
net/eth/e10 z Intel e1000/e1000e Ethernet
net/eth/igc z Intel i225/i226 Ethernet
net/eth/rtl c Realtek RTL8169 Ethernet
net/eth/r25 z Realtek 2.5G Ethernet
net/eth/bge z Broadcom NetXtreme Ethernet
net/eth/vne z virtio network device
net/wfi/iwl p Intel Wi-Fi
net/wfi/ath z Qualcomm Atheros Wi-Fi
net/wfi/mtk z MediaTek Wi-Fi
net/wfi/rtw z Realtek Wi-Fi
net/wfi/brc z Broadcom Wi-Fi
net/blt/hci z Bluetooth HCI core
net/blt/btu z Bluetooth over USB
net/blt/bta z Bluetooth over UART
gpu/fbf z UEFI and DTB framebuffer
gpu/vrt z virtio GPU
gpu/own z own GPU
gpu/int z Intel integrated GPU
gpu/amd z AMD GPU
dsp/edp z embedded DisplayPort panel
dsp/hdm z HDMI output
dsp/bkl z backlight control
usb/hst/xhc z xHCI host controller
usb/hst/ehc z EHCI host controller
usb/hst/ohc z OHCI host controller
usb/hst/uhc z UHCI host controller
usb/hst/dwc z DesignWare USB controller
usb/hub/hub z USB hub
usb/hid/gen z USB HID class
usb/sto/msc z USB mass storage
usb/aud/uac z USB audio class
usb/ser/cdc z USB CDC serial
usb/eth/cdn z USB Ethernet
usb/cam/uvc z USB video class
inp/kbd d Keyboard
inp/msu z Mouse
inp/tpd z Touchpad
inp/tch z Touchscreen
inp/i2h z I2C-HID transport
inp/btn z Lid and power buttons
aud/hda z Intel HD Audio
aud/i2s z I2S audio
aud/vsn z virtio sound
aud/cdc z Audio codec
fsy/fat z FAT12/16/32
fsy/ext z ext2/3/4
fsy/iso z ISO 9660
fsy/ntf z NTFS
fsy/tmp z tmpfs
fsy/own z Own file system
bus/pce z PCI Express
bus/i2c z I2C controller
bus/spi z SPI controller
bus/gpi z GPIO controller
bus/smb z SMBus
bus/ecn z Embedded controller
bus/vio z virtio transport
pwr/bat z Battery
pwr/chg z Charger
pwr/acd z AC adapter
pwr/thm z Thermal sensors
pwr/fan z Fan control
pwr/cpf z CPU frequency scaling
pwr/pmc z Power management controller
ser/uar o 16550 UART
ser/pl0 z PL011 UART
sen/acl z Accelerometer
sen/als z Ambient light sensor
sen/gyr z Gyroscope
sen/tmp z Temperature sensor
cam/csi z MIPI CSI camera
tmr/rtc z Real-time clock
tmr/wdt z Watchdog
sec/tpm z TPM
sec/rng z Random number generator
EOF

dr $D/_tpl
mk $D/_tpl/zig/{drv.toml,README.md,build.zig,src/main.zig}
mk $D/_tpl/rust/{drv.toml,README.md,Cargo.toml,src/lib.rs}
mk $D/_tpl/c/{drv.toml,README.md,Makefile,src/main.c}
mk $D/_tpl/cpp/{drv.toml,README.md,CMakeLists.txt,src/main.cpp}
mk $D/_tpl/d/{drv.toml,README.md,dub.sdl,source/app.d}
mk $D/_tpl/odin/{drv.toml,README.md,src/main.odin}

mk $N/srv/gfx/Cargo.toml $N/srv/gfx/src/{main,srv,win,buf,cmp,inp,prt}.rs
mk $N/srv/vfs/Cargo.toml $N/srv/vfs/src/{main,node,mnt,fid,cch,rpc}.rs
mk $N/srv/net/Cargo.toml $N/srv/net/src/{main,eth,arp,ip4,ip6,tcp,udp,sck}.rs
mk $N/srv/ini/Cargo.toml $N/srv/ini/src/{main,svc,dep,rst}.rs
mk $N/srv/drm/Cargo.toml $N/srv/drm/src/{main,man,ldr,mat,cap,hlt}.rs

mk $N/lib/std/Cargo.toml $N/lib/std/src/{lib,io,fs,thr,sync,time,env,prc}.rs
mk $N/lib/clb/Makefile $N/lib/clb/src/{stdio,stdlib,string,malloc,syscall,pthread}.c
mk $N/lib/ipc/rs/Cargo.toml $N/lib/ipc/rs/src/{lib,gen}.rs
mk $N/lib/ipc/zig/build.zig $N/lib/ipc/zig/src/ipc.zig
mk $N/lib/ipc/c/include/ipc.h $N/lib/ipc/c/src/ipc.c
mk $N/lib/ipc/cpp/include/ipc.hpp
mk $N/lib/ipc/d/source/ipc.d
mk $N/lib/ipc/odin/src/ipc.odin
mk $N/lib/gui/Cargo.toml $N/lib/gui/src/{lib,wid,lay,thm,evt,rnd}.rs

for a in tdo doc pdf sht prs mkd cod str; do mk $N/app/$a/README.md; dr $N/app/$a/src; done

mk $N/doc/spc/{abi,ipc,cap,drv-toml,gfx}.md
mk $N/doc/dec/{0001-microkernel,0002-languages,0003-naming}.md
mk $N/doc/nam/abbreviations.md
mk $N/doc/arc/overview.md

mk $N/tls/bld/{mkimg,mkiso,qemu}.sh
mk $N/tls/gen/idl/{ipc,cap,sys}.idl
mk $N/tls/gen/bindings/Cargo.toml $N/tls/gen/bindings/src/{main,rs,zig,c,cpp,d,odin}.rs
mk $N/tls/gen/drvlist/Cargo.toml $N/tls/gen/drvlist/src/{main,pci,ws}.rs
mk $N/tls/gen/tree/Cargo.toml $N/tls/gen/tree/src/main.rs
dr $N/tls/lnt/layout $N/tls/lnt/langs $N/tls/lnt/deps $N/tls/mig

mk $N/tst/qem/{x86,rv6,smoke}.sh
dr $N/tst/bnc/mem $N/tst/bnc/sch $N/tst/bnc/ipc $N/tst/unt/krn $N/tst/unt/lib
mk $N/tst/cnf/Cargo.toml $N/tst/cnf/src/{main,ipc,cap,dma,irq,free}.rs
mk $N/tst/hrd/README.md

dr $N/dst/iso $N/dst/pkg $N/dst/inst $N/dem $N/web $N/not
mk $N/.github/workflows/{x86,rv6,lint}.yml

{
  echo '[workspace]'
  echo 'resolver = "2"'
  echo 'members = ['
  (cd $N && find . -name Cargo.toml -not -path ./Cargo.toml -not -path '*/_tpl/*' -printf '%h\n' | sed 's|^\./||' | sort | sed 's|.*|    "&",|')
  echo ']'
} > $N/Cargo.toml

printf '# CTRL-F-0rg3\n\nCopyright (C) 2026 CTRL-F-0rg3\n\nLicense: GPL-3.0-only (see LICENSE).\nException: krn/net is BSD-2-Clause (see krn/net/LICENSE).\n' > $N/README.md

curl -fsSL https://www.gnu.org/licenses/gpl-3.0.txt -o $N/LICENSE || echo "UWAGA: nie pobrano GPLv3, pobierz recznie do $N/LICENSE"

cat > $K/net/LICENSE <<'EOF'
BSD 2-Clause License

Copyright (c) 2026 CTRL-F-0rg3

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EOF

find $N -type f -empty -print0 | while IFS= read -r -d '' f; do
  b=${f##*/}
  L=GPL-3.0-only
  case $f in "$K/net/"*) L=BSD-2-Clause;; esac
  case $b in
    Cargo.toml)
      n=$(dirname "$f" | sed "s|^$N/||; s|/|-|g")
      printf '[package]\nname = "%s"\nversion = "0.1.0"\nedition = "2021"\nauthors = ["CTRL-F-0rg3"]\nlicense = "%s"\n' "$n" "$L" > "$f";;
    *.rs|*.c|*.h|*.cpp|*.hpp|*.zig|*.d|*.odin|*.idl|*.sdl) hdr "$f" '//' "$L";;
    *.S) printf '/* SPDX-FileCopyrightText: 2026 CTRL-F-0rg3 */\n/* SPDX-License-Identifier: %s */\n' "$L" > "$f";;
    *.sh|*.yml|*.toml|Makefile|CMakeLists.txt) hdr "$f" '#' "$L";;
  esac
done

find $N -type d -empty -exec touch {}/.gitkeep \;

git add -A
git commit -m "Add new/ skeleton, drivers and licenses"
