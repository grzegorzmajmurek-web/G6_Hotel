#include "Database.h"
#include "Hotel.h"
#include "Room.h"
#include "Reservation.h"

// MySQL Connector/C++ - klasyczne API JDBC.
// Sciezki do tych naglowkow i bibliotek (mysqlcppconn) sa konfigurowane w Visual Studio.
// W niektorych wersjach konektora naglowki znajduja sie w podkatalogu "jdbc/",
// np. <jdbc/mysql_driver.h> - wtedy nalezy dostosowac ponizsze include.
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/datatype.h>

#include <iostream>
#include <vector>
#include <memory>

Database::Database() = default;
Database::~Database() = default;

bool Database::Connect(const std::string& host,
                       const std::string& user,
                       const std::string& password,
                       const std::string& schema)
{
    try
    {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        m_connection.reset(driver->connect(host, user, password));
        m_connection->setSchema(schema);
        std::cout << "[DB] Polaczono z baza danych \"" << schema << "\".\n";
        return true;
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad polaczenia z baza: " << e.what() << "\n";
        m_connection.reset();
        return false;
    }
}

void Database::LoadInto(Hotel& hotel)
{
    if (!m_connection) return;

    try
    {
        std::unique_ptr<sql::Statement> stmt(m_connection->createStatement());

        // --- Katalog pokoi ---
        {
            std::unique_ptr<sql::ResultSet> rs(
                stmt->executeQuery("SELECT room_number, type FROM pokoje ORDER BY room_number"));
            while (rs->next())
            {
                int number = rs->getInt("room_number");
                std::string type = rs->getString("type").c_str();
                try { hotel.AddRoom(MakeRoom(type, number)); }
                catch (const std::exception& ex)
                {
                    std::cout << "[DB] Pomijam pokoj " << number << ": " << ex.what() << "\n";
                }
            }
        }

        // --- Rezerwacje (najpierw zbieramy wiersze, potem doczytujemy uslugi) ---
        struct ResRow
        {
            int id;
            int roomNumber;
            int guests;
            bool promo;
            std::string start;
            std::string end;
            std::string guestName;
        };
        std::vector<ResRow> rows;

        {
            std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(
                "SELECT r.id, r.room_number, r.guests, r.promo_applied, "
                "r.visiting_start, r.visiting_end, k.name, k.lastname "
                "FROM rezerwacje r LEFT JOIN klienci k ON r.klient_id = k.id ORDER BY r.id"));
            while (rs->next())
            {
                ResRow row;
                row.id = rs->getInt("id");
                row.roomNumber = rs->getInt("room_number");
                row.guests = rs->getInt("guests");
                row.promo = rs->getInt("promo_applied") != 0;
                row.start = rs->getString("visiting_start").c_str();
                row.end = rs->getString("visiting_end").c_str();

                std::string name = rs->isNull("name") ? std::string("Nieznany") : std::string(rs->getString("name").c_str());
                std::string last = rs->isNull("lastname") ? std::string() : std::string(rs->getString("lastname").c_str());
                row.guestName = last.empty() ? name : (name + " " + last);
                rows.push_back(std::move(row));
            }
        }

        int maxId = 0;
        std::unique_ptr<sql::PreparedStatement> svc(m_connection->prepareStatement(
            "SELECT service_type, spa_sessions, meal_plan, pool_days, pool_sunbed, "
            "pool_towel, promo_code, promo_discount FROM uslugi WHERE rezerwacja_id = ?"));

        for (const auto& row : rows)
        {
            auto room = hotel.FindRoom(row.roomNumber);
            if (!room)
            {
                std::cout << "[DB] Pomijam rezerwacje #" << row.id
                          << " - brak pokoju " << row.roomNumber << " w katalogu.\n";
                continue;
            }

            std::vector<std::unique_ptr<Service>> services;
            svc->setInt(1, row.id);
            std::unique_ptr<sql::ResultSet> srs(svc->executeQuery());
            while (srs->next())
            {
                ServiceRecord rec;
                rec.type = srs->getString("service_type").c_str();
                if (rec.type == "SPA")
                {
                    rec.spaSessions = srs->getInt("spa_sessions");
                }
                else if (rec.type == "MEAL")
                {
                    std::string mp = srs->getString("meal_plan").c_str();
                    rec.mealPlan = (mp == "HALFBOARD") ? 1 : (mp == "FULLBOARD") ? 2 : 0;
                }
                else if (rec.type == "POOL")
                {
                    rec.poolDays = srs->getInt("pool_days");
                    rec.poolSunbed = srs->getInt("pool_sunbed") != 0;
                    rec.poolTowel = srs->getInt("pool_towel") != 0;
                }
                else if (rec.type == "PROMO")
                {
                    rec.promoCode = srs->getString("promo_code").c_str();
                    rec.promoDiscount = srs->getDouble("promo_discount");
                }

                try { services.push_back(MakeServiceFromRecord(rec)); }
                catch (const std::exception& ex)
                {
                    std::cout << "[DB] Pomijam usluge rezerwacji #" << row.id << ": " << ex.what() << "\n";
                }
            }

            try
            {
                Date from = Date::Parse(row.start);
                Date to = Date::Parse(row.end);
                hotel.RestoreReservation(row.id, room, row.guestName, row.guests,
                                         from, to, std::move(services), row.promo);
                if (row.id > maxId) maxId = row.id;
            }
            catch (const std::exception& ex)
            {
                std::cout << "[DB] Pomijam rezerwacje #" << row.id << ": " << ex.what() << "\n";
            }
        }

        hotel.SetNextReservationId(maxId + 1);
        std::cout << "[DB] Wczytano dane z bazy.\n";
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad wczytywania danych: " << e.what() << "\n";
    }
}

int Database::GetRoomDbId(int roomNumber)
{
    std::unique_ptr<sql::PreparedStatement> ps(
        m_connection->prepareStatement("SELECT id FROM pokoje WHERE room_number = ?"));
    ps->setInt(1, roomNumber);
    std::unique_ptr<sql::ResultSet> rs(ps->executeQuery());
    if (rs->next()) return rs->getInt("id");
    return -1;
}

int Database::InsertClient(const std::string& guestName)
{
    std::string name = guestName;
    std::string last;
    auto pos = guestName.find(' ');
    if (pos != std::string::npos)
    {
        name = guestName.substr(0, pos);
        last = guestName.substr(pos + 1);
    }

    std::unique_ptr<sql::PreparedStatement> ps(
        m_connection->prepareStatement("INSERT INTO klienci (name, lastname) VALUES (?, ?)"));
    ps->setString(1, name);
    if (last.empty()) ps->setNull(2, sql::DataType::VARCHAR);
    else ps->setString(2, last);
    ps->executeUpdate();

    std::unique_ptr<sql::Statement> st(m_connection->createStatement());
    std::unique_ptr<sql::ResultSet> rs(st->executeQuery("SELECT LAST_INSERT_ID() AS id"));
    if (rs->next()) return rs->getInt("id");
    return -1;
}

int Database::InsertReservation(const std::string& guestName,
                                int roomNumber,
                                int guests,
                                const Date& from,
                                const Date& to,
                                double price)
{
    if (!m_connection) return -1;
    try
    {
        int clientId = InsertClient(guestName);
        int roomDbId = GetRoomDbId(roomNumber);

        std::unique_ptr<sql::PreparedStatement> ps(m_connection->prepareStatement(
            "INSERT INTO rezerwacje (klient_id, room_id, room_number, guests, "
            "reservation_price, promo_applied, visiting_start, visiting_end) "
            "VALUES (?, ?, ?, ?, ?, 0, ?, ?)"));
        if (clientId > 0) ps->setInt(1, clientId); else ps->setNull(1, sql::DataType::INTEGER);
        if (roomDbId > 0) ps->setInt(2, roomDbId); else ps->setNull(2, sql::DataType::INTEGER);
        ps->setInt(3, roomNumber);
        ps->setInt(4, guests);
        ps->setDouble(5, price);
        ps->setString(6, from.ToString());
        ps->setString(7, to.ToString());
        ps->executeUpdate();

        std::unique_ptr<sql::Statement> st(m_connection->createStatement());
        std::unique_ptr<sql::ResultSet> rs(st->executeQuery("SELECT LAST_INSERT_ID() AS id"));
        if (rs->next()) return rs->getInt("id");
        return -1;
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad zapisu rezerwacji: " << e.what() << "\n";
        return -1;
    }
}

void Database::DeleteReservation(int reservationId)
{
    if (!m_connection) return;
    try
    {
        std::unique_ptr<sql::PreparedStatement> ps(
            m_connection->prepareStatement("DELETE FROM rezerwacje WHERE id = ?"));
        ps->setInt(1, reservationId);
        ps->executeUpdate();
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad usuwania rezerwacji: " << e.what() << "\n";
    }
}

void Database::InsertService(int reservationId, const ServiceRecord& rec)
{
    if (!m_connection) return;
    try
    {
        std::unique_ptr<sql::PreparedStatement> ps(m_connection->prepareStatement(
            "INSERT INTO uslugi (rezerwacja_id, service_type, spa_sessions, meal_plan, "
            "pool_days, pool_sunbed, pool_towel, promo_code, promo_discount) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"));
        ps->setInt(1, reservationId);
        ps->setString(2, rec.type);

        if (rec.type == "SPA") ps->setInt(3, rec.spaSessions);
        else ps->setNull(3, sql::DataType::INTEGER);

        if (rec.type == "MEAL")
        {
            const char* mp = (rec.mealPlan == 1) ? "HALFBOARD" : (rec.mealPlan == 2) ? "FULLBOARD" : "BREAKFAST";
            ps->setString(4, mp);
        }
        else ps->setNull(4, sql::DataType::VARCHAR);

        if (rec.type == "POOL")
        {
            ps->setInt(5, rec.poolDays);
            ps->setInt(6, rec.poolSunbed ? 1 : 0);
            ps->setInt(7, rec.poolTowel ? 1 : 0);
        }
        else
        {
            ps->setNull(5, sql::DataType::INTEGER);
            ps->setNull(6, sql::DataType::INTEGER);
            ps->setNull(7, sql::DataType::INTEGER);
        }

        if (rec.type == "PROMO")
        {
            ps->setString(8, rec.promoCode);
            ps->setDouble(9, rec.promoDiscount);
        }
        else
        {
            ps->setNull(8, sql::DataType::VARCHAR);
            ps->setNull(9, sql::DataType::DECIMAL);
        }

        ps->executeUpdate();
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad zapisu uslugi: " << e.what() << "\n";
    }
}

void Database::DeletePromoServices(int reservationId)
{
    if (!m_connection) return;
    try
    {
        std::unique_ptr<sql::PreparedStatement> ps(m_connection->prepareStatement(
            "DELETE FROM uslugi WHERE rezerwacja_id = ? AND service_type = 'PROMO'"));
        ps->setInt(1, reservationId);
        ps->executeUpdate();
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad usuwania kodu rabatowego: " << e.what() << "\n";
    }
}

void Database::UpdateReservation(int reservationId, double price, bool promoApplied)
{
    if (!m_connection) return;
    try
    {
        std::unique_ptr<sql::PreparedStatement> ps(m_connection->prepareStatement(
            "UPDATE rezerwacje SET reservation_price = ?, promo_applied = ? WHERE id = ?"));
        ps->setDouble(1, price);
        ps->setInt(2, promoApplied ? 1 : 0);
        ps->setInt(3, reservationId);
        ps->executeUpdate();
    }
    catch (const sql::SQLException& e)
    {
        std::cout << "[DB] Blad aktualizacji rezerwacji: " << e.what() << "\n";
    }
}
