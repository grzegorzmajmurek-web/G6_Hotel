#include "Hotel.h"
#include "SeasonalPricing.h"
#include <iostream>
#include <iomanip>
 
Hotel::Hotel(std::string name) : m_name(std::move(name)) {}
 
void Hotel::AddRoom(std::shared_ptr<Room> room)
{
    m_rooms.push_back(std::move(room));
}
 
std::shared_ptr<Room> Hotel::FindRoom(int number) const
{
    for (const auto& r : m_rooms)
        if (r->GetNumber() == number) return r;
    return nullptr;
}
 
Reservation* Hotel::FindReservation(int id)
{
    for (const auto& r : m_reservations)
        if (r->GetId() == id) return r.get();
    return nullptr;
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
 
int Hotel::CreateReservation(int roomNumber,
                             const std::string& guestName,
                             int guests,
                             const Date& from,
                             const Date& to)
{
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
        auto res = std::unique_ptr<Reservation>(
            new Reservation(m_nextReservationId++, room, guestName, guests, from, to));
        int id = res->GetId();
        m_reservations.push_back(std::move(res));
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
    if (!r) return false;
    r->AddService(std::move(service));
    return true;
}
 
void Hotel::PrintRooms() const
{
    std::cout << "== Pokoje w hotelu \"" << m_name << "\" ==\n";
    for (const auto& r : m_rooms)
        std::cout << "  " << r->Describe() << "\n";
}
 
void Hotel::PrintReservations() const
{
    std::cout << "== Rezerwacje ==\n";
    if (m_reservations.empty()) { std::cout << "  (brak)\n"; return; }
    for (const auto& r : m_reservations) r->Print();
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
 
    for (int d = 1; d <= days; ++d)
    {
        Date day{ year, month, d };
        bool busy = false;
        for (const auto& r : m_reservations)
        {
            if (r->GetRoomNumber() != roomNumber) continue;
            if (r->GetCheckIn() <= day && day < r->GetCheckOut())
            {
                busy = true; break;
            }
        }
        std::cout << "  " << day.ToString()
<< " [" << SeasonalPricing::GetSeasonName(SeasonalPricing::GetSeason(day)) << "]"
<< " - " << (busy ? "ZAJETY" : "wolny") << "\n";
    }
}