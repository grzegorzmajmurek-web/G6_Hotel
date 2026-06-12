#pragma once
#include <string>
#include <memory>
#include <stdexcept>
#include <iosfwd>

class Room
{
public:
    Room(int number, int capacity) : m_number(number), m_capacity(capacity)
    {
        if (number <= 0)
            throw std::invalid_argument("Numer pokoju musi byc liczba dodatnia");
        if (capacity < 1 || capacity > 10)
            throw std::invalid_argument("Pojemnosc pokoju musi byc w zakresie 1-10");
    }
    virtual ~Room() = default;
 
    int GetNumber() const { return m_number; }
    int GetCapacity() const { return m_capacity; }
 
    virtual double GetBasePricePerNight() const = 0;
    virtual std::string GetType() const = 0;
    virtual std::string Describe() const;

    // Dwa pokoje sa "takie same", gdy maja ten sam numer.
    bool operator==(const Room& other) const { return m_number == other.m_number; }
    bool operator!=(const Room& other) const { return !(*this == other); }
 
protected:
    int m_number;
    int m_capacity;
};

// Wypisanie pokoju (korzysta z wirtualnej metody Describe()).
std::ostream& operator<<(std::ostream& os, const Room& room);
 
class StandardRoom : public Room
{
public:
    StandardRoom(int number) : Room(number, 2) {}
    double GetBasePricePerNight() const override { return 200.0; }
    std::string GetType() const override { return "Standard"; }
};
 
class DeluxeRoom : public Room
{
public:
    DeluxeRoom(int number) : Room(number, 3) {}
    double GetBasePricePerNight() const override { return 350.0; }
    std::string GetType() const override { return "Deluxe"; }
};
 
class Suite : public Room
{
public:
    Suite(int number) : Room(number, 4) {}
    double GetBasePricePerNight() const override { return 600.0; }
    std::string GetType() const override { return "Suite"; }
};

// Fabryka tworzaca odpowiednia podklase pokoju na podstawie nazwy typu
// (uzywana przy wczytywaniu katalogu pokoi z bazy danych).
std::shared_ptr<Room> MakeRoom(const std::string& type, int number);