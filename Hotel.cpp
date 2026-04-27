// Hotel.cpp : Punkt wejscia programu konsolowego - rezerwacje hotelowe.
 
#include "Hotel.h"
#include "Room.h"
#include "Service.h"
#include "Date.h"
#include <iostream>
#include <limits>
#include <memory>
#include <string>
 
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
        int roomNo = ReadInt("Numer pokoju: ");
        std::cout << "Imie i nazwisko gosc: ";
        std::string name;
        std::cin >> std::ws;
        std::getline(std::cin, name);
        int guests = ReadInt("Liczba gosci: ");
        Date from = ReadDate("Data przyjazdu (YYYY-MM-DD): ");
        Date to   = ReadDate("Data wyjazdu  (YYYY-MM-DD): ");
 
        int id = hotel.CreateReservation(roomNo, name, guests, from, to);
        if (id > 0) std::cout << "Utworzono rezerwacje o ID " << id << ".\n";
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