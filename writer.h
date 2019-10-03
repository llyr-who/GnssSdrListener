#ifndef WRIT_H
#define WRIT_H

#include <mutex>
#include <ostream>
#include <string>

#include "gnss_synchro.pb.h"
#include "monitor_pvt.pb.h"
#include "sat_pvt.pb.h"

class Writer {
    mutable std::mutex m;
    std::ostream* m_out;

public:
    Writer(std::ostream& os) : m_out{&os} {};

    void write(std::map<int, gnss_sdr::GnssSynchro>& channels) {

        {
            std::lock_guard<std::mutex> lk(m);
            // locked now we can write
            std::ostream& output = *m_out;
            for (const auto& c : channels) {
                // clang-format off
                // IqData
                auto sync = c.second;
                output << "IqData, " << " "
                       << sync.very_early()
                       << '\n';
            }
            output << std::endl;
        }
        // unlocked
    }

    void write(gnss_sdr::MonitorPvt& monitor) {

        {
            std::lock_guard<std::mutex> lk(m);
            std::ostream& output = *m_out;
            output << "monitor" << monitor.pos_x() << std::endl;
        }
    }

    void write(gnss_sdr::SatPvt& sat) {
        {

        }
    }
};

#endif
