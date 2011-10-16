#pragma once

#include "defs.h"

#include <algorithm>
#include <iostream>

#include <cassert>

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
