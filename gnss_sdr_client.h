#ifndef GNSS_SDR_CLIENT_H_
#define GNSS_SDR_CLIENT_H_

#include <boost/asio.hpp>
#include "gnss_synchro.pb.h"
#include "monitor_pvt.pb.h"
#include "sat_pvt.pb.h"

class Gnss_Sdr_Client {
public:
    Gnss_Sdr_Client(const unsigned short synchro_port,
                    const unsigned short monitor_port,
                    const unsigned short sat_port);
    bool read_gnss_synchro(gnss_sdr::Observables& stocks);
    bool read_gnss_monitor(gnss_sdr::MonitorPvt& monitor);
    bool read_gnss_sat(gnss_sdr::SatPvt& sat);
    void populate_channels(gnss_sdr::Observables& stocks);
    bool print_table();
    // bool write...(...)
private:
    boost::asio::io_service io_service;
    boost::system::error_code error;
    // for synchro
    boost::asio::ip::udp::endpoint syncro_endpoint;
    boost::asio::ip::udp::socket syncro_socket;
    gnss_sdr::Observables stocks;
    std::map<int, gnss_sdr::GnssSynchro> channels;
    // for monitor
    boost::asio::ip::udp::endpoint monitor_endpoint;
    boost::asio::ip::udp::socket monitor_socket;
    gnss_sdr::MonitorPvt monitor;
    // for sats
    boost::asio::ip::udp::endpoint sat_endpoint;
    boost::asio::ip::udp::socket sat_socket;
    gnss_sdr::SatPvt sat;
};

#endif
