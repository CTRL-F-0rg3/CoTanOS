new/arc/rv6/drv/dp/
├── drv.toml               # Manifest sterownika dla jądra TrangorgeOS / CoTanOS
├── Makefile               # Skrypt kompilacji C + C++ z odpowiednimi flagami (-Iinclude)
├── README.md              # Opis architektoniczny sterownika DisplayPort
├── include/
│   ├── dp_internal.h      # Prywatne nagłówki rejestrów MMIO tylko dla rejestrów C
│   └── share/
│       ├── dp_abi.h       # Wspólny nagłówek C/C++: ABI, kody błędów, struktury ramek/EDID
│       └── dp_api.hpp     # Interfejs wyższy C++ dla zewnętrznych serwisów / wyższych warstw
└── src/
    ├── core_io.c          # Fundament C: bezpośrednie odczyty/zapisy MMIO, linia AUX, rejestry
    └── dp_controller.cpp  # Warstwa C++: logika maszyny stanów DisplayPort, obróbka EDID
