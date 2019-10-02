#ifndef GNSS_SDR_CLIENT_H_
#define GNSS_SDR_CLIENT_H_

#include <boost/asio.hpp>
#include <thread>
#include "gnss_synchro.pb.h"
#include "monitor_pvt.pb.h"
#include "sat_pvt.pb.h"
#include "queue.h"

class Gnss_Sdr_Client {
public:
    Gnss_Sdr_Client(const unsigned short synchro_port,
                    const unsigned short monitor_port,
                    const unsigned short sat_port);
    ~Gnss_Sdr_Client();
    bool read_gnss_synchro(gnss_sdr::Observables& stocks);
    bool read_gnss_monitor(gnss_sdr::MonitorPvt& monitor);
    bool read_gnss_sat(gnss_sdr::SatPvt& sat);
    void populate_channels(gnss_sdr::Observables& stocks);
private:
    boost::asio::io_service io_service;
    boost::system::error_code error;
    // for synchro
    boost::asio::ip::udp::endpoint synchro_endpoint;
    boost::asio::ip::udp::socket synchro_socket;
    gnss_sdr::Observables stocks;
    std::map<int, gnss_sdr::GnssSynchro> channels;
    std::thread synchro_thread;
    void synchro_task();
    // for monitor
    boost::asio::ip::udp::endpoint monitor_endpoint;
    boost::asio::ip::udp::socket monitor_socket;
    gnss_sdr::MonitorPvt monitor;
    std::thread monitor_thread;
    void monitor_task();
    // for sats
    boost::asio::ip::udp::endpoint sat_endpoint;
    boost::asio::ip::udp::socket sat_socket;
    gnss_sdr::SatPvt sat;
    std::thread sat_thread;
    void sat_task();

    // thread-safe queue
    Queue<string> q;
    // consumer function that pops of the queue
    void consumer() {
        // ...
    }
};

#endif
