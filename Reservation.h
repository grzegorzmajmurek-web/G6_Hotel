#pragma once
#include "Date.h"
#include "Room.h"
#include "Service.h"
#include <memory>
#include <vector>
#include <string>
 
class Reservation
{
public:
    Reservation(int id,
                std::shared_ptr<Room> room,
                std::string guestName,
                int guests,
                Date checkIn,
                Date checkOut);
 
    int GetId() const { return m_id; }
    const Date& GetCheckIn() const { return m_checkIn; }
    const Date& GetCheckOut() const { return m_checkOut; }
    const Room& GetRoom() const { return *m_room; }
    int GetRoomNumber() const { return m_room->GetNumber(); }
    const std::string& GetGuestName() const { return m_guestName; }
    int GetGuests() const { return m_guests; }
 
    void AddService(std::unique_ptr<Service> service);
 
    int Nights() const;
    double CalculateRoomCost() const;    // z cennikiem sezonowym per doba
    double CalculateServicesCost() const;
    double CalculateTotal() const;
 
    void Print() const;
 
private:
    int m_id;
    std::shared_ptr<Room> m_room;
    std::string m_guestName;
    int m_guests;
    Date m_checkIn;
    Date m_checkOut;
    std::vector<std::unique_ptr<Service>> m_services;
};