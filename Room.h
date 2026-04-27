#pragma once
#include <string>
#include <memory>
 
class Room
{
public:
    Room(int number, int capacity) : m_number(number), m_capacity(capacity) {}
    virtual ~Room() = default;
 
    int GetNumber() const { return m_number; }
    int GetCapacity() const { return m_capacity; }
 
    virtual double GetBasePricePerNight() const = 0;
    virtual std::string GetType() const = 0;
    virtual std::string Describe() const;
 
protected:
    int m_number;
    int m_capacity;
};
 
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