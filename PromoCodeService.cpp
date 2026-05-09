#include "PromoCodeService.h"

PromoCodeService::PromoCodeService(std::string codeName, double discountValue)
    : m_codeName(std::move(codeName)), m_discountValue(discountValue)
{
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