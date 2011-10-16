#include "defs.h"
#include "em/door.h"

#include <algorithm>
#include <fstream>
#include <iterator>

#include "bill_generator.h"

// Генератор счетов.

void print_usage(const char* program) {
    std::cout
        << "Usage: " << program << " <sorted-file> <bill-file>"
        << std::endl;

    std::cout << "sorted-file\tinput file" << std::endl;
    std::cout << "bill-file\toutput file" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    // Открываем отсортированный файлик.
    em::TFile sorted_file(argv[1], O_RDONLY);
    const em::door<LogEntry> entries(sorted_file, 0, sorted_file.size());
    
    // Генерируем счета.
    std::fstream out(argv[2], std::ios::out);
    std::for_each(entries.begin(), entries.end(), ProduceBill(out));

    return 0;
}

