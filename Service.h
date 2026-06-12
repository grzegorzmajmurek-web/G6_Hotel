#pragma once
#include <string>
#include <memory>
#include <stdexcept>

// Plaski opis uslugi uzywany do zapisu/odczytu z bazy danych (tabela `uslugi`).
// Pola sa wypelniane zaleznie od `type`:
//   "SPA"   -> spaSessions
//   "MEAL"  -> mealPlan (0=Breakfast, 1=HalfBoard, 2=FullBoard)
//   "POOL"  -> poolDays, poolSunbed, poolTowel
//   "PROMO" -> promoCode, promoDiscount
struct ServiceRecord
{
    std::string type;
    int spaSessions = 0;
    int mealPlan = 0;
    int poolDays = 0;
    bool poolSunbed = false;
    bool poolTowel = false;
    std::string promoCode;
    double promoDiscount = 0.0;
};

class Service
{
public:
    virtual ~Service() = default;
    virtual std::string GetName() const = 0;
    virtual double GetCost(int nights, int guests) const = 0;
    virtual std::unique_ptr<Service> Clone() const = 0;

    // Serializacja uslugi do postaci zapisywalnej w bazie danych.
    virtual ServiceRecord ToRecord() const = 0;
};

// Fabryka odtwarzajaca usluge na podstawie rekordu z bazy danych.
std::unique_ptr<Service> MakeServiceFromRecord(const ServiceRecord& rec);
 
class SpaService : public Service
{
public:
    explicit SpaService(int sessions) : m_sessions(sessions)
    {
        if (sessions < 1 || sessions > 100)
            throw std::invalid_argument("Liczba sesji SPA musi byc w zakresie 1-100");
    }
    std::string GetName() const override;
    double GetCost(int nights, int guests) const override;
    std::unique_ptr<Service> Clone() const override;
    ServiceRecord ToRecord() const override;

    int GetSessions() const { return m_sessions; }

private:
    int m_sessions;
    static constexpr double PricePerSession = 180.0;
};
 
class MealService : public Service
{
public:
    enum class Plan { Breakfast, HalfBoard, FullBoard };
 
    explicit MealService(Plan plan) : m_plan(plan) {}
    std::string GetName() const override;
    double GetCost(int nights, int guests) const override;
    std::unique_ptr<Service> Clone() const override;
    ServiceRecord ToRecord() const override;

    Plan GetPlan() const { return m_plan; }

private:
    Plan m_plan;
    double PricePerPersonPerNight() const;
};

class PoolService : public Service
{
public:
    PoolService(int days, bool sunbed, bool towel) : m_days(days), m_sunbed(sunbed), m_towel(towel)
    {
        if (days < 1)
            throw std::invalid_argument("Liczba dni korzystania z basenu musi byc co najmniej 1");
    }
    std::string GetName() const override;
    double GetCost(int nights, int guests) const override;
    std::unique_ptr<Service> Clone() const override;
    ServiceRecord ToRecord() const override;

    int GetDays() const { return m_days; }
    bool HasSunbed() const { return m_sunbed; }
    bool HasTowel() const { return m_towel; }

private:
    int m_days;
    bool m_sunbed;
    bool m_towel;
};