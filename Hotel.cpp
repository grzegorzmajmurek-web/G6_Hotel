// Hotel.cpp : Punkt wejscia programu konsolowego - rezerwacje hotelowe.
 
#include "Hotel.h"
#include "Room.h"
#include "Service.h"
#include "Date.h"
#include "PromoCodeService.h"
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <cctype>
 
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
<< "6. Anuluj rezerwacje\n"
<< "7. Zastosuj kod rabatowy\n"
<< "8. Usun kod rabatowy\n"
<< "0. Wyjscie\n"
<< "Wybor: ";
    }
 
    Date ReadDate(const std::string& prompt)
    {
        while (true)
        {
            std::cout << prompt;
            std::string s;
            if (!(std::cin >> s))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            try { return Date::Parse(s); }
            catch (const std::exception& ex) { std::cout << ex.what() << "\n"; }
        }
    }
 
    int ReadInt(const std::string& prompt, int min = std::numeric_limits<int>::lowest(), int max = std::numeric_limits<int>::max())
    {
        while (true)
        {
            std::cout << prompt;
            int x;
            if (std::cin >> x)
            {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (x >= min && x <= max) return x;
                std::cout << "Wartosc musi byc w przedziale [" << min << " - " << max << "].\n";
            }
            else
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Podaj poprawna liczbe calkowita.\n";
            }
        }
    }

    std::string ReadString(const std::string& prompt, size_t maxLen = 100)
    {
        while (true)
        {
            std::cout << prompt;
            std::string s;
            std::cin >> std::ws;
            std::getline(std::cin, s);
            // Trim trailing whitespace
            auto last = s.find_last_not_of(" \t\n\r");
            if (last != std::string::npos) s = s.substr(0, last + 1);
            // Trim leading whitespace
            auto first = s.find_first_not_of(" \t\n\r");
            if (first != std::string::npos) s = s.substr(first);
            else s.clear();

            if (s.empty()) { std::cout << "Pole nie moze byc puste.\n"; continue; }
            if (s.length() > maxLen) { std::cout << "Tekst zbyt dlugi (max " << maxLen << " znakow).\n"; continue; }
            return s;
        }
    }
 
    void ListRoomsFlow(const Hotel& hotel)
    {
        std::cout << "1. Pokaz wszystkie pokoje (katalog)\n"
                  << "2. Sprawdz dostepnosc pokoi w terminie\n";
        int choice = ReadInt("Wybor: ", 1, 2);
        if (choice == 1)
        {
            hotel.PrintRooms();
        }
        else
        {
            Date from = ReadDate("Data od (YYYY-MM-DD): ");
            Date to = ReadDate("Data do (YYYY-MM-DD): ");
            if (!(from < to)) { std::cout << "Data 'do' musi byc pozniejsza niz 'od'!\n"; return; }
            std::cout << "\n== Status pokoi w terminie " << from.ToString() << " -> " << to.ToString() << " ==\n";
            for (const auto& r : hotel.GetRooms())
            {
                bool avail = hotel.IsRoomAvailable(r->GetNumber(), from, to);
                std::cout << "  " << r->Describe() << " | Status: " << (avail ? "WOLNY" : "ZAJETY") << "\n";
            }
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
        if (!hotel.HasReservations())
        {
            std::cout << "Brak rezerwacji w systemie. Najpierw utworz rezerwacje.\n";
            return;
        }

        int resId = ReadInt("ID rezerwacji: ", 1);

        if (hotel.FindReservation(resId) == nullptr)
        {
            std::cout << "Nie znaleziono rezerwacji o podanym ID.\n";
            return;
        }

        auto* resPtr = hotel.FindReservation(resId);
        int stayNights = resPtr->Nights();

        std::cout << "Typ uslugi: 1) SPA  2) Posilki  3) Basen : ";
        int t = ReadInt("", 1, 3);
        std::unique_ptr<Service> svc;
        if (t == 1)
        {
            int maxSessions = std::min(100, stayNights * 3);
            if (maxSessions < 1) maxSessions = 1;
            int sessions = ReadInt("Liczba sesji SPA (1-" + std::to_string(maxSessions) + "): ", 1, maxSessions);
            svc.reset(new SpaService(sessions));
        }
        else if (t == 2)
        {
            std::cout << "Plan: 1) Sniadania  2) HB  3) FB : ";
            int p = ReadInt("", 1, 3);
            MealService::Plan plan = MealService::Plan::Breakfast;
            if (p == 2) plan = MealService::Plan::HalfBoard;
            else if (p == 3) plan = MealService::Plan::FullBoard;
            svc.reset(new MealService(plan));
        }
        else if (t == 3)
        {
            int days = ReadInt("Liczba dni korzystania z basenu (1-" + std::to_string(stayNights) + "): ", 1, stayNights);
            int s = ReadInt("Czy wypozyczyc lezak (20 PLN/dzien)? 1-Tak, 0-Nie : ", 0, 1);
            int w = ReadInt("Czy wypozyczyc recznik (10 PLN/dzien)? 1-Tak, 0-Nie : ", 0, 1);
            svc.reset(new PoolService(days, s == 1, w == 1));
        }
        else { std::cout << "Nieznany typ.\n"; return; }
 
        hotel.AddServiceToReservation(resId, std::move(svc));
        std::cout << "Dodano usluge.\n";
    }
 
    void NewReservationFlow(Hotel& hotel)
    {
        std::cout << "--- Nowa Rezerwacja ---\n";
        Date from = ReadDate("Data przyjazdu (YYYY-MM-DD): ");
        Date to   = ReadDate("Data wyjazdu  (YYYY-MM-DD): ");
        if (!(from < to))
        {
            std::cout << "Blad: Data wyjazdu musi byc pozniejsza niz data przyjazdu!\n";
            return;
        }

        std::cout << "\nDla kogo jest rezerwacja?\n"
                  << "1. Pojedyncza osoba (1 gosc)\n"
                  << "2. Para (2 gosci)\n"
                  << "3. Rodzina/Grupa (3-4 gosci)\n";
        int type = ReadInt("Wybierz opcje (1-3): ", 1, 3);
        int guests = 1;
        if (type == 1) guests = 1;
        else if (type == 2) guests = 2;
        else if (type == 3) guests = ReadInt("Podaj dokladna liczbe gosci (3-4): ", 3, 4);

        std::cout << "\n== Lista pokoi dla " << guests << " osob w terminie " << from.ToString() << " -> " << to.ToString() << " ==\n";
        bool anyAvailable = false;
        for (const auto& r : hotel.GetRooms())
        {
            if (r->GetCapacity() >= guests)
            {
                bool avail = hotel.IsRoomAvailable(r->GetNumber(), from, to);
                std::cout << "  " << r->Describe() << " | Status: " << (avail ? "WOLNY" : "ZAJETY") << "\n";
                if (avail) anyAvailable = true;
            }
        }

        if (!anyAvailable)
        {
            std::cout << "Brak wolnych pokoi spelniajacych kryteria w tym terminie.\n";
            return;
        }

        int roomNo = ReadInt("\nWybierz numer WOLNEGO pokoju z listy powyzej: ", 1);
        auto room = hotel.FindRoom(roomNo);
        if (!room)
        {
            std::cout << "Blad: Nie istnieje pokoj o numerze " << roomNo << "!\n";
            return;
        }
        if (room->GetCapacity() < guests)
        {
            std::cout << "Blad: Pokoj #" << roomNo << " jest za maly dla " << guests << " osob.\n";
            return;
        }
        if (!hotel.IsRoomAvailable(roomNo, from, to))
        {
            std::cout << "Blad: Pokoj #" << roomNo << " jest ZAJETY w wybranym terminie!\n";
            return;
        }

        std::string name = ReadString("Imie i nazwisko goscia: ", 100);
 
        int id = hotel.CreateReservation(roomNo, name, guests, from, to);
        if (id > 0) std::cout << "Utworzono rezerwacje o ID " << id << ".\n";
    }
 
    void CalendarFlow(const Hotel& hotel)
    {
        int roomNo = ReadInt("Numer pokoju: ", 1);
        int year   = ReadInt("Rok: ", 1970, 2100);
        int month  = ReadInt("Miesiac (1-12): ", 1, 12);
        hotel.PrintCalendarForRoom(roomNo, year, month);
    }

    void CancelReservationFlow(Hotel& hotel)
    {
        if (!hotel.HasReservations())
        {
            std::cout << "Brak rezerwacji w systemie. Nie ma czego anulowac.\n";
            return;
        }

        int id = ReadInt("ID rezerwacji do anulowania: ", 1);
        if (hotel.CancelReservation(id))
            std::cout << "Rezerwacja #" << id << " zostala pomyslnie anulowana.\n";
        else
            std::cout << "Nie znaleziono rezerwacji o podanym ID.\n";
    }

    void ApplyPromoCodeFlow(Hotel& hotel)
    {
        if (!hotel.HasReservations())
        {
            std::cout << "Brak rezerwacji w systemie.\n";
            return;
        }

        int resId = ReadInt("ID rezerwacji: ", 1);
        auto* res = hotel.FindReservation(resId);
        if (!res)
        {
            std::cout << "Nie znaleziono rezerwacji o podanym ID.\n";
            return;
        }

        if (res->HasPromoCode())
        {
            std::cout << "Blad: Do tej rezerwacji zostal juz przypisany kod rabatowy. Mozna uzyc tylko jednego.\n";
            return;
        }

        std::cout << "Dostepne kody: RODZINA (-10%), LATO (-50 PLN), DLUGIPOBYT (do -20%)\n";
        std::string code = ReadString("Podaj kod rabatowy: ", 50);
        for (auto& c : code) c = toupper(c);

        double discountAmount = 0.0;
        if (code == "RODZINA")
        {
            if (res->GetGuests() < 3)
            {
                std::cout << "Blad: Kod RODZINA jest dostepny tylko dla rezerwacji rodzinnych (minimum 3 osoby).\n";
                return;
            }
            discountAmount = res->CalculateTotal() * 0.10;
        }
        else if (code == "LATO")
        {
            discountAmount = 50.0;
        }
        else if (code == "DLUGIPOBYT")
        {
            int n = res->Nights();
            if (n >= 14) discountAmount = res->CalculateTotal() * 0.20; // 20% za 14+ dni
            else if (n >= 7) discountAmount = res->CalculateTotal() * 0.10; // 10% za 7+ dni
            else if (n >= 3) discountAmount = res->CalculateTotal() * 0.05; // 5% za 3+ dni
            else
            {
                std::cout << "Blad: Kod DLUGIPOBYT jest wazny tylko dla pobytow na minimum 3 noce.\n";
                return;
            }
        }
        else
        {
            std::cout << "Nieznany lub niewazny kod rabatowy.\n";
            return;
        }

        // Zapobiegamy zejściu ceny całej rezerwacji poniżej zera
        if (res->CalculateTotal() - discountAmount < 0) discountAmount = res->CalculateTotal(); 

        hotel.AddServiceToReservation(resId, std::unique_ptr<Service>(new PromoCodeService(code, discountAmount)));
        res->SetPromoCodeApplied(true);
        std::cout << "Zastosowano kod rabatowy! Odliczono " << discountAmount << " PLN od obecnej kwoty.\n";
    }

    void RemovePromoCodeFlow(Hotel& hotel)
    {
        if (!hotel.HasReservations())
        {
            std::cout << "Brak rezerwacji w systemie.\n";
            return;
        }

        int resId = ReadInt("ID rezerwacji: ", 1);
        auto* res = hotel.FindReservation(resId);
        if (!res)
        {
            std::cout << "Nie znaleziono rezerwacji o podanym ID.\n";
            return;
        }

        if (!res->HasPromoCode()) { std::cout << "Ta rezerwacja nie ma przypisanego kodu rabatowego.\n"; return; }

        res->RemovePromoCodes();
        std::cout << "Kod rabatowy zostal pomyslnie usuniety! Mozesz teraz dodac nowy w menu (Opcja 7).\n";
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
            std::cout << "Nieznana opcja. Podaj poprawna cyfre.\n";
            continue;
        }

        // Czyszczenie bufora na wypadek "brudnego" wejścia z literami po cyfrze
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
 
        switch (choice)
        {
            case 1: ListRoomsFlow(hotel); break;
            case 2: NewReservationFlow(hotel); break;
            case 3: AddServiceFlow(hotel); break;
            case 4: hotel.PrintReservations(); break;
            case 5: CalendarFlow(hotel); break;
            case 6: CancelReservationFlow(hotel); break;
            case 7: ApplyPromoCodeFlow(hotel); break;
            case 8: RemovePromoCodeFlow(hotel); break;
            case 0: std::cout << "Do widzenia.\n"; return 0;
            default: std::cout << "Nieznana opcja.\n"; break;
        }
    }
}