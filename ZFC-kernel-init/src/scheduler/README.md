# CoTanOS Scheduler (Donor Code)

## Pochodzenie i Licencja (Origin & License)
Kod w tym katalogu został początkowo skopiowany z projektu **xv6-riscv** (MIT License).
Oryginalny kod zachowuje swoje nagłówki licencyjne MIT na górze każdego pliku. Zgodnie z wymogami licencji MIT, oryginalny copyright musi pozostać w plikach, dopóki nie zostaną one w całości przepisane.

## Strategia Rozwoju (GPLv3 Migration Plan)
Ten moduł służy jako dojrzała baza (reference implementation) dla podsystemu zarządzania procesami w CoTanOS.
Zgodnie z planem rozwoju, kod z tego katalogu będzie **stopniowo zastępowany** własną implementacją CoTanOS (licencjonowaną na GPLv3).

### Zasady modyfikacji (Contribution Rules):
1. **Nie usuwaj** oryginalnych nagłówków licencyjnych MIT z plików, dopóki plik nie zostanie w całości przepisany od zera.
2. Nowe bloki kodu (GPLv3) dodawane do tych plików muszą być wyraźnie oznaczone komentarzami, np.:
   /*
    * --- BEGIN CoTanOS GPLv3 ---
    * Author: CTRL-F-0rg3
    * Description: Custom CoTanOS implementation replacing xv6 logic.
    */
   // kod
   /* --- END CoTanOS GPLv3 --- */
3. Docelowo, po przepisaniu całego pliku i usunięciu kodu MIT, nagłówek MIT zostanie usunięty i zastąpiony standardowym nagłówkiem GPLv3 CoTanOS.

## Pliki źródłowe (provenance)
| Plik | Źródło | Modyfikacje CoTanOS |
|---|---|---|
| proc.c / proc.h | xv6-riscv (MIT) | brak / w trakcie |
| swtch.S | xv6-riscv (MIT) | brak |
| spinlock.c / .h | xv6-riscv (MIT) | brak |
| printk.c | xv6-riscv (MIT) | brak |
| types.h, param.h, riscv.h, defs.h, string.c | xv6-riscv (MIT) | brak |
