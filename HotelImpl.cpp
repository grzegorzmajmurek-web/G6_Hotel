#include "Hotel.h"
#include "Database.h"
#include "SeasonalPricing.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
 
Hotel::Hotel(std::string name) : m_name(std::move(name)) {}
 
void Hotel::AddRoom(std::shared_ptr<Room> room)
{
    if (!room)
        throw std::invalid_argument("Nie mozna dodac pustego pokoju");
    if (FindRoom(room->GetNumber()))
        throw std::invalid_argument("Pokoj o numerze " + std::to_string(room->GetNumber()) + " juz istnieje");
    m_rooms.push_back(std::move(room));
}
 
std::shared_ptr<Room> Hotel::FindRoom(int number) const
{
    auto it = std::find_if(m_rooms.begin(), m_rooms.end(),
                           [number](const auto& r) { return r->GetNumber() == number; });
    return it != m_rooms.end() ? *it : nullptr;
}
 
Reservation* Hotel::FindReservation(int id)
{
    auto it = std::find_if(m_reservations.begin(), m_reservations.end(),
                           [id](const auto& r) { return r->GetId() == id; });
    return it != m_reservations.end() ? it->get() : nullptr;
}

bool Hotel::CancelReservation(int id)
{
    auto it = std::find_if(m_reservations.begin(), m_reservations.end(),
                           [id](const auto& r) { return r->GetId() == id; });
    if (it != m_reservations.end())
    {
        if (m_db) m_db->DeleteReservation(id);
        m_reservations.erase(it);
        return true;
    }
    return false;
}

bool Hotel::HasReservations() const
{
    return !m_reservations.empty();
}

bool Hotel::IsRoomAvailable(int roomNumber, const Date& from, const Date& to) const
{
    for (const auto& r : m_reservations)
    {
        if (r->GetRoomNumber() != roomNumber) continue;
        if (RangesOverlap(r->GetCheckIn(), r->GetCheckOut(), from, to))
            return false;
    }
    return true;
}
 
static bool IsValidGuestName(const std::string& name)
{
    if (name.empty()) return false;
    return name.find_first_not_of(" \t\n\r") != std::string::npos;
}

int Hotel::CreateReservation(int roomNumber,
                             const std::string& guestName,
                             int guests,
                             const Date& from,
                             const Date& to)
{
    if (!IsValidGuestName(guestName))
    {
        std::cout << "Blad: Imie i nazwisko goscia nie moze byc puste.\n";
        return -1;
    }
    if (!(from < to))
    {
        std::cout << "Blad: Data wyjazdu musi byc pozniejsza niz data przyjazdu.\n";
        return -1;
    }
    auto room = FindRoom(roomNumber);
    if (!room)
    {
        std::cout << "Brak pokoju o numerze " << roomNumber << ".\n";
        return -1;
    }
    if (!IsRoomAvailable(roomNumber, from, to))
    {
        std::cout << "Pokoj " << roomNumber << " zajety we wskazanym terminie.\n";
        return -1;
    }
    try
    {
        int newId = m_nextReservationId;

        // Jesli dostepna jest baza danych, to ona nadaje identyfikator (AUTO_INCREMENT).
        if (m_db)
        {
            // Tymczasowa rezerwacja do wyliczenia ceny pokoju (sezonowo).
            Reservation temp(0, room, guestName, guests, from, to);
            int dbId = m_db->InsertReservation(guestName, roomNumber, guests, from, to, temp.CalculateTotal());
            if (dbId <= 0)
            {
                std::cout << "Blad: nie udalo sie zapisac rezerwacji w bazie danych.\n";
                return -1;
            }
            newId = dbId;
        }

        auto res = std::unique_ptr<Reservation>(
            new Reservation(newId, room, guestName, guests, from, to));

        int id = res->GetId();
        m_reservations.push_back(std::move(res));
        if (id >= m_nextReservationId) m_nextReservationId = id + 1;
        return id;
    }
    catch (const std::exception& ex)
    {
        std::cout << "Blad tworzenia rezerwacji: " << ex.what() << "\n";
        return -1;
    }
}
 
bool Hotel::AddServiceToReservation(int reservationId, std::unique_ptr<Service> service)
{
    auto* r = FindReservation(reservationId);
    if (!r || !service) return false;

    if (m_db)
    {
        m_db->InsertService(reservationId, service->ToRecord());
    }

    r->AddService(std::move(service));

    if (m_db)
    {
        m_db->UpdateReservation(reservationId, r->CalculateTotal(), r->HasPromoCode());
    }
    return true;
}

bool Hotel::RemovePromoFromReservation(int reservationId)
{
    auto* r = FindReservation(reservationId);
    if (!r) return false;

    r->RemovePromoCodes();

    if (m_db)
    {
        m_db->DeletePromoServices(reservationId);
        m_db->UpdateReservation(reservationId, r->CalculateTotal(), r->HasPromoCode());
    }
    return true;
}

void Hotel::RestoreReservation(int id,
                               std::shared_ptr<Room> room,
                               const std::string& guestName,
                               int guests,
                               const Date& from,
                               const Date& to,
                               std::vector<std::unique_ptr<Service>> services,
                               bool promoApplied)
{
    auto res = std::unique_ptr<Reservation>(
        new Reservation(id, room, guestName, guests, from, to));
    for (auto& s : services)
        res->AddService(std::move(s));
    res->SetPromoCodeApplied(promoApplied);
    m_reservations.push_back(std::move(res));
}
 
void Hotel::PrintRooms() const
{
    std::cout << "== Pokoje w hotelu \"" << m_name << "\" ==\n";
    for (const auto& r : m_rooms)
        std::cout << "  " << *r << "\n";
}
 
void Hotel::PrintReservations() const
{
    std::cout << "== Rezerwacje ==\n";
    if (m_reservations.empty()) { std::cout << "  (brak)\n"; return; }
    for (const auto& r : m_reservations) std::cout << *r;
}
 
void Hotel::PrintCalendarForRoom(int roomNumber, int year, int month) const
{
    auto room = FindRoom(roomNumber);
    if (!room) { std::cout << "Brak pokoju " << roomNumber << ".\n"; return; }
 
    std::cout << "Kalendarz pokoju #" << roomNumber
<< " - " << year << "-" << std::setw(2) << std::setfill('0') << month << "\n";
 
    Date first{ year, month, 1 };
    if (!first.IsValid()) { std::cout << "Nieprawidlowy miesiac.\n"; return; }
 
    Date probe = first;
    int days = 0;
    while (probe.month == month) { ++days; probe = Date::FromSerial(probe.ToSerial() + 1); }
 
    // OPTYMALIZACJA: Przefiltrujmy rezerwacje tylko do tych, 
    // które faktycznie dotyczą tego pokoju w interesującym nas miesiącu.
    Date last{ year, month, days };
    std::vector<Reservation*> roomReservations;
    for (const auto& r : m_reservations)
    {
        if (r->GetRoomNumber() == roomNumber && 
            RangesOverlap(r->GetCheckIn(), r->GetCheckOut(), first, Date::FromSerial(last.ToSerial() + 1)))
        {
            roomReservations.push_back(r.get());
        }
    }

    for (int d = 1; d <= days; ++d)
    {
        Date day{ year, month, d };
        bool busy = false;
        for (const auto* r : roomReservations)
        {
            if (r->GetCheckIn() <= day && day < r->GetCheckOut())
            {
                busy = true; break;
            }
        }
        std::cout << "  " << day
<< " [" << SeasonalPricing::GetSeasonName(SeasonalPricing::GetSeason(day)) << "]"
<< " - " << (busy ? "ZAJETY" : "wolny") << "\n";
    }
}