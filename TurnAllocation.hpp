#ifndef TURN_ALLOCATION
#define TURN_ALLOCATION

#include "GlobalUsing.h"

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>

#define TURN_TCP 0x0001
#define TURN_UDP 0x0002

struct FiveTuple {
    QHostAddress client_ipv4;
    std::uint16_t client_port;
    QHostAddress server_ipv4;
    std::uint16_t server_port;
    std::uint16_t protocol;
};

struct TurnAllocation {
    TurnAllocation(FiveTuple ft, std::size_t tte) {
        five_tuple = ft;
        time_to_expiry = tte;
    }
    FiveTuple five_tuple;
    std::size_t time_to_expiry;
    //QHostAddress relayed_ipv4;
    //std::uint16_t relayed_port;
};

#endif