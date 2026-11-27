#!/usr/bin/env python3
import os
import sys
from pathlib import Path

SKIP_DIRS = {
    '.git', 'old0.1', 'old0.2', 'lsd',
    'ZFC', 'userspace', 'ct-std', 'ctlib',
    'de', 'loomcom', 'lunacom', 'titanus', 'drivers',
    'build', '__pycache__', '.cache', 'node_modules', 'target',
}

SKIP_EXTENSIONS = {
    '.img', '.o', '.elf', '.bin', '.so', '.a', '.fd',
    '.exe', '.dll', '.dylib', '.pyc', '.pyo',
    '.png', '.jpg', '.jpeg', '.gif', '.bmp', '.ico',
    '.pdf', '.zip', '.tar', '.gz', '.bz2', '.xz',
    '.class', '.jar', '.war',
}

SKIP_FILES = {'.DS_Store', 'Thumbs.db', 'desktop.ini'}

SKIP_SUFFIXES = {'.bak', '.tmp', '.swp', '.swo', '~', '.orig', '.rej'}

LANGUAGE_MAP = {
    '.py': 'python', '.rs': 'rust', '.c': 'c', '.h': 'c',
    '.cpp': 'cpp', '.hpp': 'cpp', '.cc': 'cpp', '.cxx': 'cpp',
    '.zig': 'zig', '.asm': 'nasm', '.S': 'asm', '.s': 'asm',
    '.sh': 'bash', '.bash': 'bash', '.zsh': 'zsh',
    '.md': 'markdown', '.txt': 'text', '.ld': 'ld',
    '.toml': 'toml', '.json': 'json', '.yaml': 'yaml', '.yml': 'yaml',
    '.xml': 'xml', '.html': 'html', '.css': 'css',
    '.js': 'javascript', '.ts': 'typescript', '.java': 'java',
    '.go': 'go', '.rb': 'ruby', '.lua': 'lua',
    '.conf': 'ini', '.cfg': 'ini', '.ini': 'ini',
    '.lock': 'toml',
    'Makefile': 'makefile', 'makefile': 'makefile',
    'GNUmakefile': 'makefile',
}

MAX_FILE_SIZE = 1 * 1024 * 1024

def get_language(filepath):
    name = filepath.name
    if name in LANGUAGE_MAP:
        return LANGUAGE_MAP[name]
    return LANGUAGE_MAP.get(filepath.suffix.lower(), 'text')

def is_text_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8', errors='strict') as f:
            f.read(1024)
        return True
    except (UnicodeDecodeError, OSError):
        return False

def should_skip(filepath, root):
    rel = filepath.relative_to(root)
    for part in rel.parts:
        if part in SKIP_DIRS:
            return True
    if filepath.name in SKIP_FILES:
        return True
    if filepath.suffix.lower() in SKIP_EXTENSIONS:
        return True
    for suffix in SKIP_SUFFIXES:
        if filepath.name.endswith(suffix):
            return True
    return False

def collect_files(root):
    files = []
    for dirpath, dirnames, filenames in os.walk(root):
        dirpath = Path(dirpath)
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for filename in sorted(filenames):
            filepath = dirpath / filename
            if should_skip(filepath, root):
                continue
            if not filepath.is_file():
                continue
            if filepath.stat().st_size > MAX_FILE_SIZE:
                continue
            if not is_text_file(filepath):
                continue
            files.append(filepath)
    return sorted(files)

def dump_project(root, output):
    files = collect_files(root)
    print(f"Znaleziono {len(files)} plikow tekstowych.")
    with open(output, 'w', encoding='utf-8') as out:
        out.write('# CoTanOS Project Dump\n\n')
        out.write(f'Wygenerowano z: `{root.resolve()}`\n\n')
        out.write(f'Liczba plikow: {len(files)}\n\n')
        out.write('---\n\n')
        out.write('## Spis tresci\n\n')
        for filepath in files:
            rel = filepath.relative_to(root)
            out.write(f'- `{rel}`\n')
        out.write('\n---\n\n')
        for filepath in files:
            rel = filepath.relative_to(root)
            lang = get_language(filepath)
            try:
                with open(filepath, 'r', encoding='utf-8') as f:
                    content = f.read()
            except Exception as e:
                content = f'[BLAD ODCZYTU: {e}]'
            content = content.replace('```', '\\`\\`\\`')
            out.write(f'## `{rel}`\n\n')
            out.write(f'```{lang}\n')
            out.write(content)
            if not content.endswith('\n'):
                out.write('\n')
            out.write('```\n\n')
            out.write('---\n\n')
    print(f"Zapisano do: {output}")
    print(f"Rozmiar: {output.stat().st_size / 1024:.1f} KB")

if __name__ == '__main__':
    root = Path(sys.argv[1]) if len(sys.argv) > 1 else Path('.')
    output = Path(sys.argv[2]) if len(sys.argv) > 2 else Path('project_dump.md')
    if not root.exists():
        print(f'Blad: katalog `{root}` nie istnieje.')
        sys.exit(1)
    dump_project(root, output)
