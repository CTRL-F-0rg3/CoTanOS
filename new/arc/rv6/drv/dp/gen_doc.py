#!/usr/bin/env python3
import os

root_dir = "."
files_info = []

for r, _, files in os.walk(root_dir):
    for f in files:
        if f.endswith((".c", ".cpp", ".h", ".hpp", ".toml", ".md")):
            path = os.path.normpath(os.path.join(r, f))
            if "html" in path or ".git" in path:
                continue
            with open(path, "r", encoding="utf-8", errors="ignore") as file_obj:
                lines = file_obj.readlines()
            files_info.append({"path": path, "lines": len(lines)})

markdown_content = "# Dokumentacja techniczna sterownika DisplayPort / HDMI (`rv6`)\n\n"
markdown_content += "## 1. Struktura komponentu i statystyki kodu\n\n"
markdown_content += "| Ścieżka pliku | Liczba linii | Rola w architekturze |\n"
markdown_content += "|---|---|---|\n"

total_lines = 0
for item in sorted(files_info, key=lambda x: x["path"]):
    p = item["path"]
    l = item["lines"]
    total_lines += l

    role = "Nieznana"
    if "core_io.c" in p:
        role = "Surowe operacje MMIO, bariery i obsługa fizyczna"
    elif "core_io.h" in p:
        role = "Fundament C, makra RISC-V i wskaźniki volatile"
    elif "dp_controller.hpp" in p:
        role = "Maszyna stanów, klasy C++ i specyfikacja MSA"
    elif "dp_controller.cpp" in p:
        role = "Implementacja logiki kontrolera C++"
    elif ".h" in p or ".hpp" in p:
        role = "Nagłówek ABI / interfejs współdzielony"
    elif ".toml" in p:
        role = "Manifest konfiguracyjny sterownika"
    elif ".md" in p:
        role = "Dokumentacja tekstowa / drzewo"

    markdown_content += f"| `{p}` | {l} | {role} |\n"

markdown_content += f"\n**Łączna liczba linii kodu w komnponencie:** `{total_lines}`\n\n"
markdown_content += "## 2. Architektura i Powiązania\n\n"
markdown_content += "- **Warstwa C (`core_io.*`):** Odpowiada za bezpośredni dostęp do przestrzeni adresowej MMIO, emulację instrukcji `fence` dla RISC-V oraz operacje buforowe.\n"
markdown_content += "- **Warstwa C++ (`dp_controller.*`):** Enkapsuluje zarządzanie pasmem, procedury Link Training oraz obsługę tablic EDID i struktur strumienia wideo.\n"

with open("DRIVER_REFERENCE.md", "w", encoding="utf-8") as out:
    out.write(markdown_content)

print(f"[+] Pomyślnie wygenerowano plik dokumentacji: DRIVER_REFERENCE.md (Łącznie linii: {total_lines})")
