#include "class_file.hpp"
#include "methods.hpp"
#include "interpreter.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Użycie: ./jvm <plik.class>\n";
        return 1;
    }

    ClassFile cls;
    // Musimy delikatnie rozszerzyć ClassFile, żeby wczytywało metody po Constant Pool
    // Wczytajmy plik ręcznie lub dopiszmy funkcję odczytu metod bezpośrednio do ClassFile:
    try {
        ByteReader reader(argv[1]);
        cls.magic = reader.read_u4();
        if (cls.magic != 0xCAFEBABE) {
            std::cerr << "Błędny nagłówek klasy!\n";
            return 1;
        }
        cls.minor_version = reader.read_u2();
        cls.major_version = reader.read_u2();
        cls.constant_pool_count = reader.read_u2();
        
        // Pomińmy parowanie constant pool z poprzedniego kodu lub wklej je tutaj:
        // (załóżmy, że wczytałeś Constant Pool tak jak wcześniej)...
        
        // Po Constant Pool mamy access_flags, this_class, super_class, interfaces:
        reader.read_u2(); // access_flags
        reader.read_u2(); // this_class
        reader.read_u2(); // super_class
        
        uint16_t interfaces_count = reader.read_u2();
        for(int i=0; i<interfaces_count; ++i) reader.read_u2();

        // Pola (fields)
        uint16_t fields_count = reader.read_u2();
        for(int i=0; i<fields_count; ++i) {
            reader.read_u2(); reader.read_u2(); reader.read_u2(); // access, name, desc
            uint16_t f_attr_count = reader.read_u2();
            for(int j=0; j<f_attr_count; ++j) {
                reader.read_u2();
                uint32_t len = reader.read_u4();
                reader.read_bytes(len);
            }
        }

        // Metody!
        std::vector<MethodInfo> methods;
        parse_methods(reader, cls, methods);

        std::cout << "[JVM] Znaleziono " << methods.size() << " metod w klasie.\n";

        // Szukamy metody np. "main" i ją uruchamiamy testowo
        Interpreter interp;
        for (const auto& m : methods) {
            // Nazwę metody wyciągamy z Constant Pool przez m.name_index
            // (zakładając, że masz funkcję get_utf8 w klasie ClassFile)
            // Tutaj uproszczenie: odpalmy pierwszą metodę z kodem
            if (m.has_code) {
                interp.execute(m, cls);
                break;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << "\n";
    }

    return 0;
}