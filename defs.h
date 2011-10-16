#pragma once

#include <inttypes.h>
#include <ctime>
#include <iostream>

// Стоимость одной минуты разговора.
const int64_t kCentsPerMinute = 2;

// Тип события: звонок начался, звонок закончился.
enum {
    kCallStarted = 1,
    kCallEnded = 2
};

// Запись в журнале событий.
struct LogEntry {
    int64_t source;
    int64_t target;

    time_t timestamp;
    int event;
};

std::istream& operator>>(std::istream& is, LogEntry& entry) {
    is >> entry.source;
    is >> entry.target;
    is >> entry.timestamp;
    is >> entry.event;
    return is;
}

// Лексикографический порядок на тройке (source, timestamp, event).
struct SortByCaller {
    inline bool operator()(const LogEntry& lhs, const LogEntry& rhs) const {
        return (lhs.source <  rhs.source)
            || (lhs.source == rhs.source && lhs.target < rhs.target)
            || (lhs.source == rhs.source && lhs.target == rhs.target &&
                    lhs.timestamp <  rhs.timestamp)
            || (lhs.source == rhs.source && lhs.target == rhs.target && 
                    lhs.timestamp == rhs.timestamp && lhs.event < rhs.event);
    }
};

