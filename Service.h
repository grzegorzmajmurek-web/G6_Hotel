#pragma once
#include <string>
#include <memory>
 
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
    explicit SpaService(int sessions) : m_sessions(sessions) {}
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