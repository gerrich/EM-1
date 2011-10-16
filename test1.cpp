#include "defs.h"
#include "bill_generator.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
//#include <limits>


// Генератор счетов.


void print_usage(const char* program) {
    std::cout
        << "Usage: " << program << " <log-file> <bill-file> [memory]"
        << std::endl;

    std::cout << " log-file\tinput file" << std::endl;
    std::cout << "bill-file\toutput file" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    // Читаем лог событий.
    std::fstream in(argv[1], std::ios::in);
    std::vector<LogEntry> entries;
    std::copy(
        std::istream_iterator<LogEntry>(in),
        std::istream_iterator<LogEntry>(),
        std::back_inserter(entries));

    // Сортируем по телефонному номеру.
    std::sort(entries.begin(), entries.end(), SortByCaller());

    // Генерируем счета.
    std::fstream out(argv[2], std::ios::out);
    std::for_each(entries.begin(), entries.end(), ProduceBill(out));

    return 0;
}

