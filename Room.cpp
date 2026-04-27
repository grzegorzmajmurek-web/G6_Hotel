#include "Room.h"
#include <sstream>
 
std::string Room::Describe() const
{
    std::ostringstream oss;
    oss << "Pokoj #" << m_number
<< " | typ: " << GetType()
<< " | osob: " << m_capacity
<< " | cena bazowa/doba: " << GetBasePricePerNight() << " PLN";
    return oss.str();
}