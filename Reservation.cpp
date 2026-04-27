#include "Reservation.h"
#include "SeasonalPricing.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
 
Reservation::Reservation(int id,
                         std::shared_ptr<Room> room,
                         std::string guestName,
                         int guests,
                         Date checkIn,
                         Date checkOut)
    : m_id(id),
      m_room(std::move(room)),
      m_guestName(std::move(guestName)),
      m_guests(guests),
      m_checkIn(checkIn),
      m_checkOut(checkOut)
{
    if (!(m_checkIn < m_checkOut))
        throw std::invalid_argument("Data wyjazdu musi byc pozniejsza od przyjazdu.");
    if (m_guests <= 0 || m_guests > m_room->GetCapacity())
        throw std::invalid_argument("Nieprawidlowa liczba gosci dla tego pokoju.");
}
 
void Reservation::AddService(std::unique_ptr<Service> service)
{
    m_services.push_back(std::move(service));
}
 
int Reservation::Nights() const
{
    return DaysBetween(m_checkIn, m_checkOut);
}
 
double Reservation::CalculateRoomCost() const
{
    double total = 0.0;
    const double base = m_room->GetBasePricePerNight();
    long from = m_checkIn.ToSerial();
    long to = m_checkOut.ToSerial();
    for (long s = from; s < to; ++s)
    {
        Date d = Date::FromSerial(s);
        total += base * SeasonalPricing::GetMultiplier(d);
    }
    return total;
}
 
double Reservation::CalculateServicesCost() const
{
    double total = 0.0;
    const int n = Nights();
    for (const auto& s : m_services)
        total += s->GetCost(n, m_guests);
    return total;
}
 
double Reservation::CalculateTotal() const
{
    return CalculateRoomCost() + CalculateServicesCost();
}
 
void Reservation::Print() const
{
    std::cout << "Rezerwacja #" << m_id
<< " | Gosc: " << m_guestName
<< " | " << m_checkIn.ToString() << " -> " << m_checkOut.ToString()
<< " (" << Nights() << " nocy)\n"
<< "   " << m_room->Describe() << "\n"
<< "   Osob: " << m_guests << "\n";
 
    if (!m_services.empty())
    {
        std::cout << "   Uslugi dodatkowe:\n";
        const int n = Nights();
        for (const auto& s : m_services)
        {
            std::cout << "     - " << s->GetName()
<< " : " << std::fixed << std::setprecision(2)
<< s->GetCost(n, m_guests) << " PLN\n";
        }
    }
 
    std::cout << std::fixed << std::setprecision(2)
<< "   Koszt pokoju (sezonowo): " << CalculateRoomCost() << " PLN\n"
<< "   Koszt uslug: " << CalculateServicesCost() << " PLN\n"
<< "   RAZEM: " << CalculateTotal() << " PLN\n";
}