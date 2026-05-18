#pragma once
#include <string>
#include <memory>
#include <stdexcept>
 
class Service
{
public:
    virtual ~Service() = default;
    virtual std::string GetName() const = 0;
    virtual double GetCost(int nights, int guests) const = 0;
    virtual std::unique_ptr<Service> Clone() const = 0;
};
 
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

private:
    int m_days;
    bool m_sunbed;
    bool m_towel;
};