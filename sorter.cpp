#include "defs.h"
#include "em/door.h"
#include "em/sort.h"

#include <algorithm>
#include <fstream>
#include <iterator>
//#include <vector>

// Генератор счетов.

void print_usage(const char* program)
{
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

    em::TFile parsed_log(argv[1], O_RDONLY);
    const em::door<LogEntry> in_door(parsed_log, 0, parsed_log.size());

    em::TFile sorted_log(argv[2], O_RDWR | O_CREAT | O_TRUNC);
    em::door<LogEntry> out_door(sorted_log, 0, 0);
  cerr << "let s copy:" << endl; 
    // Копируем данные, чтобы не испортить исходник 
  cerr << "pre copy size: " << parsed_log.size() << endl;
    std::copy(in_door.begin(), in_door.end(), out_door.begin());
  cerr << "post copy size: " << parsed_log.size() << endl;
  cerr << "post copy copy size: " << sorted_log.size() << endl;

  cerr << "let s sort:" << endl; 
    // Сортируем файлик
    em::sort<LogEntry>(sorted_log, SortByCaller());

    return 0;
}

