#include "gnss_sdr_client.h"

Gnss_Sdr_Client::Gnss_Sdr_Client(const unsigned short synchro_port,
                                 const unsigned short monitor_port,
                                 const unsigned short sat_port, Writer& w)
    : monitor_socket{io_service},
      synchro_socket{io_service},
      sat_socket{io_service},
      monitor_endpoint{boost::asio::ip::udp::v4(), monitor_port},
      synchro_endpoint{boost::asio::ip::udp::v4(), synchro_port},
      sat_endpoint{boost::asio::ip::udp::v4(), sat_port},
      w(&w){
    // open and bind the sockets
    monitor_socket.open(monitor_endpoint.protocol(), error);
    synchro_socket.open(synchro_endpoint.protocol(), error);
    sat_socket.open(sat_endpoint.protocol(), error);
    monitor_socket.bind(monitor_endpoint, error);
    synchro_socket.bind(synchro_endpoint, error);
    sat_socket.bind(sat_endpoint, error);

    // fire off threads
    synchro_thread = std::thread{&Gnss_Sdr_Client::synchro_task, this};
    monitor_thread = std::thread{&Gnss_Sdr_Client::monitor_task, this};
    sat_thread = std::thread{&Gnss_Sdr_Client::sat_task, this};
    std::cout << "The client is listening for ..." << std::endl;
    std::cout << "Gnss_Synchro on port " << synchro_port << std::endl;
    std::cout << "MonitorPvt on port " << monitor_port << std::endl;
    std::cout << "SatPvt on port " << sat_port << std::endl;
}

Gnss_Sdr_Client::~Gnss_Sdr_Client() { synchro_thread.join(); }

bool Gnss_Sdr_Client::read_gnss_monitor(gnss_sdr::MonitorPvt& monitor) {
    char buff[2000];  // Buffer for storing the received data.

    int bytes = monitor_socket.receive(boost::asio::buffer(buff));
    std::string data(&buff[0], bytes);
    return monitor.ParseFromString(data);
}

bool Gnss_Sdr_Client::read_gnss_synchro(gnss_sdr::Observables& stocks) {
    char buff[2000];  // Buffer for storing the received data.

    // This call will block until one or more bytes of data has been received.
    int bytes = synchro_socket.receive(boost::asio::buffer(buff));
    std::string data(&buff[0], bytes);
    // Deserialize a stock of Gnss_Synchro objects from the binary string.
    return stocks.ParseFromString(data);
}

bool Gnss_Sdr_Client::read_gnss_sat(gnss_sdr::SatPvt& sat) {
    char buff[2000];  // Buffer for storing the received data.

    int bytes = sat_socket.receive(boost::asio::buffer(buff));
    std::string data(&buff[0], bytes);
    return sat.ParseFromString(data);
}

void Gnss_Sdr_Client::populate_channels(gnss_sdr::Observables& stocks) {
    for (std::size_t i = 0; i < stocks.observable_size(); i++) {
        gnss_sdr::GnssSynchro ch = stocks.observable(i);
        if (ch.fs() != 0)  // Channel is valid.
        {
            channels[ch.channel_id()] = ch;
        }
    }
}

void Gnss_Sdr_Client::synchro_task() {
    while (true) {
        if (read_gnss_synchro(stocks)) {
            populate_channels(stocks);
            w->write(channels);
        }
    }
}

void Gnss_Sdr_Client::monitor_task() {
    while (true) {
        if (read_gnss_monitor(monitor)) {
            w->write(monitor);
        }
    }
}

void Gnss_Sdr_Client::sat_task() {
    while (true) {
        if (read_gnss_sat(sat)) {
            w->write(sat);
        }
    }
}

