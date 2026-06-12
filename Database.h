#pragma once
#include "Date.h"
#include "Service.h"
#include <string>
#include <memory>

// Wyprzedzajaca deklaracja - unikamy wlaczania ciezkich naglowkow konektora tutaj.
class Hotel;

namespace sql { class Connection; }

// Warstwa dostepu do bazy danych MySQL/MariaDB (XAMPP) z uzyciem
// MySQL Connector/C++ (klasyczne API JDBC: mysql_driver / cppconn).
//
// Tabele: klienci, pokoje, rezerwacje, uslugi (patrz hotelBaza.sql).
//
// Uzycie:
//   Database db;
//   if (db.Connect()) { db.LoadInto(hotel); hotel.SetDatabase(&db); }
class Database
{
public:
    Database();
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Nawiazuje polaczenie. Domyslne dane odpowiadaja standardowej instalacji XAMPP.
    bool Connect(const std::string& host = "tcp://127.0.0.1:3306",
                 const std::string& user = "root",
                 const std::string& password = "",
                 const std::string& schema = "hotel");

    bool IsConnected() const { return m_connection != nullptr; }

    // Wczytuje katalog pokoi oraz istniejace rezerwacje (wraz z uslugami) do obiektu Hotel.
    void LoadInto(Hotel& hotel);

    // Wstawia goscia (klienci) oraz rezerwacje; zwraca nowe ID rezerwacji (lub -1 przy bledzie).
    int InsertReservation(const std::string& guestName,
                          int roomNumber,
                          int guests,
                          const Date& from,
                          const Date& to,
                          double price);

    void DeleteReservation(int reservationId);

    // Zapisuje usluge dodatkowa przypisana do rezerwacji.
    void InsertService(int reservationId, const ServiceRecord& rec);

    // Usuwa wszystkie uslugi typu PROMO (kody rabatowe) z danej rezerwacji.
    void DeletePromoServices(int reservationId);

    // Aktualizuje zapisana laczna cene rezerwacji oraz flage kodu rabatowego.
    void UpdateReservation(int reservationId, double price, bool promoApplied);

private:
    int GetRoomDbId(int roomNumber);
    int InsertClient(const std::string& guestName);

    std::shared_ptr<sql::Connection> m_connection;
};
