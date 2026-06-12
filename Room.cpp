#include "Room.h"
#include <sstream>
#include <ostream>
 
std::string Room::Describe() const
{
    std::ostringstream oss;
    oss << "Pokoj #" << m_number
<< " | typ: " << GetType()
<< " | osob: " << m_capacity
<< " | cena bazowa/doba: " << GetBasePricePerNight() << " PLN";
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Room& room)
{
    return os << room.Describe();
}

std::shared_ptr<Room> MakeRoom(const std::string& type, int number)
{
    if (type == "Standard") return std::make_shared<StandardRoom>(number);
    if (type == "Deluxe")   return std::make_shared<DeluxeRoom>(number);
    if (type == "Suite")    return std::make_shared<Suite>(number);
    throw std::invalid_argument("Nieznany typ pokoju z bazy danych: " + type);
}