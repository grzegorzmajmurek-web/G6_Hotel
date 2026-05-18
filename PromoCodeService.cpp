#include "PromoCodeService.h"
#include <cmath>
#include <stdexcept>

PromoCodeService::PromoCodeService(std::string codeName, double discountValue)
    : m_codeName(std::move(codeName)), m_discountValue(discountValue)
{
    if (m_codeName.empty())
        throw std::invalid_argument("Nazwa kodu rabatowego nie moze byc pusta");
    if (m_codeName.length() > 50)
        throw std::invalid_argument("Nazwa kodu rabatowego jest zbyt dluga (max 50 znakow)");
    if (!std::isfinite(discountValue) || discountValue <= 0.0)
        throw std::invalid_argument("Wartosc rabatu musi byc liczba dodatnia");
    if (discountValue > 10000.0)
        throw std::invalid_argument("Wartosc rabatu nie moze przekraczac 10000 PLN");
}

std::string PromoCodeService::GetName() const
{
    return "Rabat (" + m_codeName + ")";
}

double PromoCodeService::GetCost(int /*nights*/, int /*guests*/) const
{
    return -m_discountValue; // Koszt ujemny = zniżka
}

std::unique_ptr<Service> PromoCodeService::Clone() const
{
    return std::unique_ptr<Service>(new PromoCodeService(*this));
}