#include "TimeCheck.h"
#include "NTPClient.h"

#include <QString>
#include <QList>
#include <QDateTime>
#include <cmath>

TimeCheck::Status TimeCheck::checkTime()
{
    // The result of checking the time with the server
    Status result = Status::NoConnection;
    bool isConnected;
    long int timeServer;
    
        // get somehow bool using config or other reading thing if needed
        if(CheckTimeFunctionalityIsNeeded)
            // Reading the parameter of the allowed time delay
            uint timeDer; // Read or insert a local time deviation in seconds, for ex.: 60
            // Reading the list of connection servers
            QList serverList; //Read or insert list of servers to connect, for ex.: "10.50.250.14,0.de.pool.ntp.org,1.de.pool.ntp.org"
            const QStringList ntpServers(serverList->value().toString().split(','));

            // Attempting to join the servers, if successful, we get the time.
            for(QList<QString>::const_iterator host(ntpServers.begin()); host != ntpServers.end(); ++host)
            {
                std::tie(isConnected, timeServer) = MultipleKinectsPlatformServer::NTPClient::RequestDatetime_UNIX(host->toStdString());

                if(isConnected)
                {
                    // Calculating the difference in seconds
                    double diff = fabs(QDateTime::currentDateTimeUtc().toTime_t() - timeServer);

                    // If the time difference is less than the permissible discrepancy, then the result is positive.
                    if(diff < timeDer->value().toDouble())
                        result = Status::Match;
                    else
                        result = Status::NotMatch;

                    // Returning the result of time matching with the first successfully connected server
                    return result;
                }
                else // There is no connection to the server
                {}
            }
        }
        else // You don't need to check the time.
            result = Status::Match;
    }
    else // There is no need for checking the time.
        result = Status::Match;

    return result;
}

