#include <boost/lexical_cast.hpp>
#include <chrono>
#include <thread>
#include "gnss_sdr_client.h"

int main(int argc, char* argv[]) {
    try {
        unsigned short port1 = boost::lexical_cast<unsigned short>(argv[1]);
        unsigned short port2 = boost::lexical_cast<unsigned short>(argv[2]);
        unsigned short port3 = port2 + 1;

        Gnss_Sdr_Client client(port1,port2,port3);
        std::cout << "The client is listening for ..." << std::endl;
        std::cout << "Gnss_Synchro on port " << port1 << std::endl;
        std::cout << "MonitorPvt on port " << port2 << std::endl;
        std::cout << "SatPvt on port " << port3 << std::endl;
        while (true) {
            client.print_table();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
