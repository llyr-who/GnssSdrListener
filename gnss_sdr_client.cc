#include "gnss_sdr_client.h"

Gnss_Sdr_Client::Gnss_Sdr_Client(const unsigned short synchro_port,
                                 const unsigned short monitor_port,
                                 const unsigned short sat_port)
    : monitor_socket{io_service},
      syncro_socket{io_service},
      sat_socket{io_service},
      monitor_endpoint{boost::asio::ip::udp::v4(), monitor_port},
      syncro_endpoint{boost::asio::ip::udp::v4(), synchro_port},
      sat_endpoint{boost::asio::ip::udp::v4(), sat_port}
{
    // open and bind the sockets
    monitor_socket.open(monitor_endpoint.protocol(), error);
    syncro_socket.open(syncro_endpoint.protocol(), error);
    sat_socket.open(sat_endpoint.protocol(), error);
    monitor_socket.bind(monitor_endpoint, error);
    syncro_socket.bind(syncro_endpoint, error);
    sat_socket(sat_endpoint, error);
}

bool Gnss_Sdr_Client::read_gnss_monitor(gnss_sdr::MonitorPvt& monitor) {
    char buff[2000];  // Buffer for storing the received data.

    // This call will block until one or more bytes of data has been received.
    int bytes = monitor_socket.receive(boost::asio::buffer(buff));
    std::string data(&buff[0], bytes);
    // Deserialize a stock of Gnss_Monitor objects from the binary string.
    return monitor.ParseFromString(data);
}

bool Gnss_Sdr_Client::read_gnss_synchro(gnss_sdr::Observables& stocks) {
    char buff[2000];  // Buffer for storing the received data.

    // This call will block until one or more bytes of data has been received.
    int bytes = syncro_socket.receive(boost::asio::buffer(buff));
    std::string data(&buff[0], bytes);
    // Deserialize a stock of Gnss_Synchro objects from the binary string.
    return stocks.ParseFromString(data);
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

bool Gnss_Sdr_Client::print_table() {
    if (read_gnss_synchro(stocks)) {
        populate_channels(stocks);
        // Print table header.
        for (const auto& c : channels) {
            auto sync = c.second;
            std::cout << "channel: " << c.first << std::endl;
            std::cout << " VE "
                      << " E "
                      << " P "
                      << " L "
                      << " VL " << std::endl;
            std::cout << sync.very_early() << " "
                      << sync.early() << " "
                      << sync.prompt() << " "
                      << sync.late() << " " 
                      << sync.very_late() << std::endl;
        }
    }
    if (read_gnss_monitor(monitor)) {
        std::cout << " X "
                  << " Y "
                  << " Z "
                  << " lat "
                  << " long "
                  << " alt " << std::endl;
        std::cout << monitor.pos_x() << " " << monitor.pos_y() << " "
                  << monitor.pos_z() << " " << monitor.latitude() << " "
                  << monitor.longitude() << " " << monitor.height()
                  << std::endl;
    }
    return true;
}

