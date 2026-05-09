// Hotel.cpp : Punkt wejscia programu konsolowego - rezerwacje hotelowe.
 
#include "Hotel.h"
#include "Room.h"
#include "Service.h"
#include "Date.h"
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
namespace
{
    void PrintMenu()
    {
        
        std::cout << "\n===== MENU =====\n"
<< "1. Lista pokoi\n"
<< "2. Nowa rezerwacja\n"
<< "3. Dodaj usluge do rezerwacji\n"
<< "4. Lista rezerwacji\n"
<< "5. Kalendarz pokoju (miesiac)\n"
<< "0. Wyjscie\n"
<< "Wybor: ";
    }
 
    Date ReadDate(const std::string& prompt)
    {
        while (true)
        {
            std::cout << prompt;
            std::string s;
            std::cin >> s;
            try { return Date::Parse(s); }
            catch (const std::exception& ex) { std::cout << ex.what() << "\n"; }
        }
    }
 
    int ReadInt(const std::string& prompt)
    {
        while (true)
        {
            std::cout << prompt;
            int x;
            if (std::cin >> x) return x;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Podaj liczbe.\n";
        }
    }
 
    void SeedRooms(Hotel & hotel)
    {
        hotel.AddRoom(std::make_shared<StandardRoom>(101));
        hotel.AddRoom(std::make_shared<StandardRoom>(102));
        hotel.AddRoom(std::make_shared<DeluxeRoom>(201));
        hotel.AddRoom(std::make_shared<DeluxeRoom>(202));
        hotel.AddRoom(std::make_shared<Suite>(301));
    }
 
    void AddServiceFlow(Hotel& hotel)
    {
        int resId = ReadInt("ID rezerwacji: ");
        std::cout << "Typ uslugi: 1) SPA  2) Posilki : ";
        int t; std::cin >> t;
        std::unique_ptr<Service> svc;
        if (t == 1)
        {
            int sessions = ReadInt("Liczba sesji SPA: ");
            svc.reset(new SpaService(sessions));
        }
        else if (t == 2)
        {
            std::cout << "Plan: 1) Sniadania  2) HB  3) FB : ";
            int p; std::cin >> p;
            MealService::Plan plan = MealService::Plan::Breakfast;
            if (p == 2) plan = MealService::Plan::HalfBoard;
            else if (p == 3) plan = MealService::Plan::FullBoard;
            svc.reset(new MealService(plan));
        }
        else { std::cout << "Nieznany typ.\n"; return; }
 
        if (hotel.AddServiceToReservation(resId, std::move(svc)))
            std::cout << "Dodano usluge.\n";
        else
            std::cout << "Nie znaleziono rezerwacji.\n";
    }
 
    void NewReservationFlow(Hotel& hotel)
    {
        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(driver->connect(
                "tcp://localhost:3306",
                "root",
                ""
            ));
            // Ustawiamy istniejące schema 'hotel' zamiast tworzenia nowej bazy
            con->setSchema("hotel");
        //int roomNo = ReadInt("Numer pokoju: ");
        //std::cout << "Imie i nazwisko gosc: ";
        //std::string name;
        //std::cin >> std::ws;
        //std::getline(std::cin, name);
        //int guests = ReadInt("Liczba gosci: ");
        //Date from = ReadDate("Data przyjazdu (YYYY-MM-DD): ");
        //Date to   = ReadDate("Data wyjazdu  (YYYY-MM-DD): ");
 // TEST
        int id = hotel.CreateReservation(101, "TEST TEST", 2, Date().Parse("2026-06-20"), Date().Parse("2026-07-20"));
        //int id = hotel.CreateReservation(roomNo, name, guests, from, to);
        std::unique_ptr<sql::Statement> id_stmt(con->createStatement());

        std::unique_ptr<sql::ResultSet> res(
            id_stmt->executeQuery(
                "SELECT id FROM rezerwacje ORDER BY id DESC LIMIT 1"
            )
        );

        int last_id = 0;

        if (res->next()) {
            last_id = res->getInt("id");
        }

        if (id > 0 && last_id < 1)
            std::cout << "Utworzono rezerwacje o ID " << id << ".\n";
        else
            std::cout << "Utworzono rezerwacje o ID " << last_id+1 << ".\n";
        
            std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement(
                "INSERT INTO klienci(name, lastname) VALUES(?, ?)"
            ));
			pstmt->setString(1, "TEST");
			pstmt->setString(2, "TEST");
			pstmt->execute();
            std::unique_ptr<sql::PreparedStatement> pstmt2(con->prepareStatement(
                "INSERT INTO rezerwacje(klient_id, room_id, visiting_start, visiting_end, reservation_price, room_number) VALUES(?, ?, ?, ?, ?, ?)"
            ));

            pstmt2->setInt(1, id);
            pstmt2->setInt(2, 1);
            pstmt2->setString(3, Date().Parse("2026-06-20").ToString());
            pstmt2->setString(4, Date().Parse("2026-07-20").ToString());
            pstmt2->setDouble(5, 0.0);
            pstmt2->setInt(6, 301);
            pstmt2->execute();
        }
        catch (sql::SQLException& e) {
            std::cout << "Blad: " << e.what() << std::endl;
        }
    }
 
    void CalendarFlow(const Hotel& hotel)
    {
        int roomNo = ReadInt("Numer pokoju: ");
        int year   = ReadInt("Rok: ");
        int month  = ReadInt("Miesiac (1-12): ");
        hotel.PrintCalendarForRoom(roomNo, year, month);
    }
}
 
int main()
{
    setlocale(LC_ALL, "pl_PL");
    Hotel hotel("Pod Gwiazdami");
    SeedRooms(hotel);
 
    std::cout << "Witaj w systemie rezerwacji hotelu \"Pod Gwiazdami\".\n";
 
    while (true)
    {
        PrintMenu();
        int choice;
        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
 
        switch (choice)
        {
            case 1: hotel.PrintRooms(); break;
            case 2: NewReservationFlow(hotel); break;
            case 3: AddServiceFlow(hotel); break;
            case 4: hotel.PrintReservations(); break;
            case 5: CalendarFlow(hotel); break;
            case 0: std::cout << "Do widzenia.\n"; return 0;
            default: std::cout << "Nieznana opcja.\n"; break;
        }
    }
}