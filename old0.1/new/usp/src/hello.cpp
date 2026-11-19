// SPDX-FileCopyrightText: 2026 CTRL-F-0rg3
// SPDX-License-Identifier: GPL-3.0-only
/*
 * CoTanOS userspace — /bin/hello-cpp (C++, libstdc++ static).
 *
 * Full C++ standard library: iostream, std::vector, std::string,
 * std::sort — the same libstdc++ as on Linux.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <unistd.h>

extern char **environ;

int main()
{
    std::vector<std::string> words = {
        "CoTanOS", "C++", "full", "libstdc++"
    };
    std::sort(words.begin(), words.end());

    std::cout << "[cpp] Hello from C++ with the full standard library:\n";
    for (const auto &w : words) {
        std::cout << "      - " << w << "\n";
    }

    char *nargv[] = {const_cast<char *>("/bin/hello-rs"), nullptr};
    execve("/bin/hello-rs", nargv, environ);
    std::cout << "[cpp] execve failed\n";
    return 0;
}
