#include "NTPClient.h"

#include <boost/asio/basic_datagram_socket.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace MultipleKinectsPlatformServer
{

    /**
     *  NTPClient
     *  @Param i_hostname - The time server host name which you are connecting to obtain the time
     *                      eg. the pool.ntp.org project virtual cluster of timeservers
     */

    std::tuple<bool, long> receiveTime(boost::asio::io_service* io_service, boost::asio::basic_datagram_socket<boost::asio::ip::udp>* socket, string* i_hostname, unsigned short* port)
    {
        std::tuple<bool, long> tblTime = std::make_tuple<bool, long int>(false, 0);
        time_t timeRecv;

        boost::asio::ip::udp::resolver resolver(*io_service);
        boost::asio::ip::udp::resolver::query query(
                        boost::asio::ip::udp::v4(),
                        *i_hostname,
                        "ntp");

        try
        {
            boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);

            socket->open(boost::asio::ip::udp::v4());

            boost::array<unsigned char, 48> sendBuf  = {010, 0, 0, 0, 0, 0, 0, 0, 0};

            boost::array<unsigned long, 1024> recvBuf;
            boost::asio::ip::udp::endpoint sender_endpoint;

            socket->send_to(boost::asio::buffer(sendBuf), receiver_endpoint);

            socket->receive_from(boost::asio::buffer(recvBuf), sender_endpoint);

            timeRecv = ntohl((time_t)recvBuf[4]);

            timeRecv -= 2208988800U; //Unix time starts from 01/01/1970 == 2208988800U

            if(timeRecv > 0)
                tblTime = std::make_tuple(true, (long)timeRecv);

            else
                tblTime = std::make_tuple(false, (long)timeRecv);

        }
        catch(std::exception& e)
        {
            tblTime = std::make_tuple(false, (long)timeRecv);
        }

        return tblTime;
    }

    void closeBlocked(boost::asio::ip::udp::socket* socket)
    {
        boost::asio::socket_base::bytes_readable command(true);
        socket->io_control(command);
        std::size_t bytes_readable = command.get();

        if(!bytes_readable)
        {
            boost::system::error_code _error_code;
            socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, _error_code);
        }
        else
            socket->close();
    }

    /**
     * RequestDatetime_UNIX()
     * @Returns long - number of seconds from the Unix Epoch start time
     */
    std::tuple<bool, long> NTPClient::RequestDatetime_UNIX(string i_hostname, unsigned short port)
    {
        std::tuple<bool, long> tblTime = std::make_tuple<bool, long int>(false, 0);
        time_t timeRecv;

        boost::asio::io_service io_service;

        boost::asio::ip::udp::socket socket(io_service);

        try
        {
            boost::asio::deadline_timer t_deadline(io_service, boost::posix_time::seconds(1));
            t_deadline.async_wait(boost::bind(closeBlocked, &socket));

            std::future<std::tuple<bool, long>> conResult = std::async(receiveTime, &io_service, &socket, &i_hostname, &port);
            io_service.run();
            socket.close();
            tblTime = conResult.get();
        }
        catch(std::exception& e)
        {
            tblTime = std::make_tuple(false, 0);
        }

        return tblTime;
    }
}
