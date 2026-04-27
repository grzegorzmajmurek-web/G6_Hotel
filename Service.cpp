#include "Service.h"

 

std::string SpaService::GetName() const
{
    return "SPA (" + std::to_string(m_sessions) + " sesji)";
}

 

double SpaService::GetCost(int /*nights*/, int /*guests*/) const
{
    return m_sessions * PricePerSession;
}

 

std::unique_ptr<Service> SpaService::Clone() const
{
    return std::unique_ptr<Service>(new SpaService(*this));
}

 

std::string MealService::GetName() const
{
    switch (m_plan)
    {
        case Plan::Breakfast: return "Posilki: sniadania";
        case Plan::HalfBoard: return "Posilki: HB (sniadanie+obiadokolacja)";
        case Plan::FullBoard: return "Posilki: FB (3 posilki)";
    }
    return "Posilki";
}

 

double MealService::PricePerPersonPerNight() const
{
    switch (m_plan)
    {
        case Plan::Breakfast: return 45.0;
        case Plan::HalfBoard: return 90.0;
        case Plan::FullBoard: return 140.0;
    }
    return 0.0;
}

 

double MealService::GetCost(int nights, int guests) const
{
    return PricePerPersonPerNight() * nights * guests;
}

 

std::unique_ptr<Service> MealService::Clone() const
{
    return std::unique_ptr<Service>(new MealService(*this));
}