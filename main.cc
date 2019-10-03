#include <boost/lexical_cast.hpp>
#include <chrono>
#include <thread>
#include "gnss_sdr_client.h"
#include "writer.h"

int main(int argc, char* argv[]) {
    unsigned short port1 = boost::lexical_cast<unsigned short>(argv[1]);
    unsigned short port2 = boost::lexical_cast<unsigned short>(argv[2]);
    unsigned short port3 = port2 + 1;

    Writer w(std::cout);

    { Gnss_Sdr_Client client(port1, port2, port3,w); }

    return 0;
}
