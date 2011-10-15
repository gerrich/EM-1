#include <inttypes.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <limits>

#include <cassert>
#include <ctime>

#include <stxxl.h>

// Стоимость одной минуты разговора.
const int64_t kCentsPerMinute = 2;

// Тип события: звонок начался, звонок закончился.
enum {
    kCallStarted = 1,
    kCallEnded = 2
};

// Запись в журнале событий.
struct LogEntry
{
    int64_t source;
    int64_t target;

    time_t timestamp;
    int event;
};

std::istream& operator>>(std::istream& is, LogEntry& entry)
{
    is >> entry.source;
    is >> entry.target;
    is >> entry.timestamp;
    is >> entry.event;
    return is;
}

// Лексикографический порядок на тройке (source, timestamp, event).
struct SortByCaller
{
    inline bool operator()(const LogEntry& lhs, const LogEntry& rhs) const
    {
        return (lhs.source <  rhs.source)
            || (lhs.source == rhs.source && lhs.target < rhs.target)
            || (lhs.source == rhs.source && lhs.target == rhs.target &&
                    lhs.timestamp <  rhs.timestamp)
            || (lhs.source == rhs.source && lhs.target == rhs.target && 
                    lhs.timestamp == rhs.timestamp && lhs.event < rhs.event);
    }

    static LogEntry min_value()
    {
        LogEntry rv;
        rv.source = std::numeric_limits<int64_t>::min();
        rv.target = std::numeric_limits<int64_t>::min();
        rv.timestamp = std::numeric_limits<time_t>::min();
        rv.event = 1;
        return rv;
    }

    static LogEntry max_value()
    {
        LogEntry rv;
        rv.source = std::numeric_limits<int64_t>::max();
        rv.target = std::numeric_limits<int64_t>::max();
        rv.timestamp = std::numeric_limits<time_t>::max();
        rv.event = 2;
        return rv;
    }
};

// Генератор счетов.
struct ProduceBill
{
    std::ostream& os;

    int64_t total;
    LogEntry last_entry;

    ProduceBill(std::ostream& os_)
        : os(os_), total(0), last_entry()
    {
        last_entry.source = -1; // Специальный номер.
    }

    void operator()(const LogEntry& entry)
    {
        if (last_entry.source == entry.source)
        {
            assert(
                (last_entry.event == kCallStarted && entry.event == kCallEnded) ||
                (last_entry.event == kCallEnded && entry.event == kCallStarted)
            );

            if (entry.event == kCallEnded) {
                total += kCentsPerMinute * 
                    (59 + entry.timestamp - last_entry.timestamp) / 60;
            }
        }
        else if (last_entry.source != -1)
        {
            assert(last_entry.event == kCallEnded);
            assert(entry.event == kCallStarted);

            os
                << last_entry.source << "; $"
                << (total / 100) << "." << (total % 100) << std::endl;

            total = 0;
        }

        last_entry = entry;
    }
};

void print_usage(const char* program)
{
    std::cout
        << "Usage: " << program << " <log-file> <bill-file> [memory]"
        << std::endl;

    std::cout << " log-file\tinput file" << std::endl;
    std::cout << "bill-file\toutput file" << std::endl;
    std::cout << "   memory\tmemory size (in MiB)" << std::endl;
}

template<typename T>
void print_internals(const T& vector)
{
    std::cout << "== Internals" << std::endl;
    std::cout << "element_size = "
        << sizeof(typename T::value_type) << " bytes" << std::endl;
    std::cout << "block_size   = "
        << T::block_size << " bytes" << std::endl;
    std::cout << "page_size    = "
        << T::page_size * T::block_size << " bytes" << std::endl;
    std::cout << "page_size    = "
        << T::page_size << " blocks per page" << std::endl;
//    std::cout << "cache_size   = "
//        << vector.numpages() << " pages" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        print_usage(argv[0]);
        return 1;
    }

    // Параметры.
    // P=1,C=2
    // P=4,C=8
    const unsigned int B = 1 * 1024;
    const unsigned int M = atol(argv[3]) * 1024 * 1024;

    typedef stxxl::VECTOR_GENERATOR<LogEntry, 1, 2, B>::result LogEntryVector;

    LogEntryVector entries;
    print_internals(entries);

    // Читаем лог событий.
    {
        std::fstream in(argv[1], std::ios::in);
        std::copy(
            std::istream_iterator<LogEntry>(in),
            std::istream_iterator<LogEntry>(),
            std::back_inserter(entries));
    }

    // Для IO-статистики.
    stxxl::stats_data stats_begin, stats_end;

    // Сортируем по телефонному номеру.
    stats_begin = *stxxl::stats::get_instance();
    {
        stxxl::sort(entries.begin(), entries.end(), SortByCaller(), M);
    }
    stats_end = *stxxl::stats::get_instance();

    std::cout << "== sort()" << std::endl
            << (stats_end - stats_begin) << std::endl;

    // Генерируем счета.
    stats_begin = *stxxl::stats::get_instance();
    {
        std::fstream out(argv[2], std::ios::out);
        stxxl::for_each(entries.begin(), entries.end(), ProduceBill(out), 2);
    }
    stats_end = *stxxl::stats::get_instance();

    std::cout << "== for_each()" << std::endl
        << (stats_end - stats_begin) << std::endl;

    return 0;
}

