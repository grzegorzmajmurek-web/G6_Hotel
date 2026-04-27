#pragma once
#include "Room.h"
#include "Reservation.h"
#include <memory>
#include <vector>
#include <string>
 
class Hotel
{
public:
    Hotel(std::string name);
 
    void AddRoom(std::shared_ptr<Room> room);
    const std::vector<std::shared_ptr<Room>>& GetRooms() const { return m_rooms; }
 
    bool IsRoomAvailable(int roomNumber, const Date& from, const Date& to) const;
 
    // Zwraca id utworzonej rezerwacji lub -1 jesli niedostepny.
    int CreateReservation(int roomNumber,
                          const std::string& guestName,
                          int guests,
                          const Date& from,
                          const Date& to);
 
    bool AddServiceToReservation(int reservationId, std::unique_ptr<Service> service);
 
    void PrintRooms() const;
    void PrintReservations() const;
    void PrintCalendarForRoom(int roomNumber, int year, int month) const;
 
    std::shared_ptr<Room> FindRoom(int number) const;
    Reservation* FindReservation(int id);
 
private:
    std::string m_name;
    std::vector<std::shared_ptr<Room>> m_rooms;
    std::vector<std::unique_ptr<Reservation>> m_reservations;
    int m_nextReservationId = 1;
};