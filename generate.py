#!/usr/bin/env python

import sys
import random

def generate_number():
    return "84957{0:05d}".format(random.randint(0, 99999))

def generate_timestamp():
    current_time = 1285240651

    while True:
        current_time += random.randint(30, 600)
        yield current_time

def generate_event(number_of_events, max_concurrent = 100):
    timestamp = generate_timestamp()
    active_events = set()

    for i in xrange(number_of_events):
        p = random.random()
        k = len(active_events)

        if (k > max_concurrent) or (k > 0 and p > 0.5):
            event = active_events.pop()
            typee = "2"
        else:
            event = ( generate_number(), generate_number() )
            typee = "1"

            active_events.add(event)

        yield event[0], event[1], str(next(timestamp)), typee

    for event in active_events:
        yield event[0], event[1], str(next(timestamp)), "2"

def main():
    if len(sys.argv) < 2:
        print >>sys.stderr, "Usage: {0} <number-of-entries>".format(sys.argv[0])
        sys.exit(1)

    events_to_generate = int(sys.argv[1])

    for event in generate_event(events_to_generate):
        print "\t".join(event)

if __name__ == "__main__":
    main()


