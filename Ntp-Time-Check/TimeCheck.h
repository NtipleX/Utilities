#ifndef TIMECHECK_H
#define TIMECHECK_H

class TimeCheck
{
public:
    enum Status
    {
        Match,       ///< Time matches
        NotMatch,     ///< Time not matches
        NoConnection  ///< No connection setup
    };

    /**
     * @brief checkTime Makes a comparison between server time and pc time
     * \details First, the list of servers is read, and then an attempt is made to connect to the queue
     * @return Result of time deviation
     */
    static Status checkTime();

};

#endif // TIMECHECK_H
