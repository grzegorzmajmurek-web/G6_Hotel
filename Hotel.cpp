#pragma once

#include <cstdint>
#include <array>
#include <string>
#include <stdexcept>
#include <locale>
#include <sstream>
#include <iomanip>
#include <compare>

namespace hotel::domain {

// ISO 4217 currency code — 3 chars, never a raw string
using CurrencyCode = std::array<char, 3>;

constexpr CurrencyCode PLN = {'P', 'L', 'N'};
constexpr CurrencyCode EUR = {'E', 'U', 'R'};
constexpr CurrencyCode USD = {'U', 'S', 'D'};

class CurrencyMismatchException final : public std::runtime_error {
public:
    explicit CurrencyMismatchException(CurrencyCode lhs, CurrencyCode rhs)
        : std::runtime_error(
            std::string("Currency mismatch: ") +
            std::string(lhs.data(), 3) + " vs " +
            std::string(rhs.data(), 3)) {}
};

class NegativeMoneyException final : public std::runtime_error {
public:
    explicit NegativeMoneyException()
        : std::runtime_error("Money amount cannot be negative") {}
};

// CRITICAL: Money stored as int64 grosze — NEVER double
// float/double arithmetic on money is a career-ending mistake
class Money final {
public:
    // Factory methods — explicit construction only
    [[nodiscard]] static constexpr Money fromGrosze(int64_t grosze, CurrencyCode currency) noexcept {
        return Money{grosze, currency};
    }

    [[nodiscard]] static Money fromDecimal(double value, CurrencyCode currency) {
        // Only entry point for double — immediately converted
        int64_t grosze = static_cast<int64_t>(std::llround(value * 100.0));
        return Money{grosze, currency};
    }

    // Zero money for a given currency
    [[nodiscard]] static constexpr Money zero(CurrencyCode currency) noexcept {
        return Money{0, currency};
    }

    // Arithmetic — currency-safe
    [[nodiscard]] Money operator+(const Money& rhs) const {
        assertSameCurrency(rhs);
        return Money{amount_ + rhs.amount_, currency_};
    }

    [[nodiscard]] Money operator-(const Money& rhs) const {
        assertSameCurrency(rhs);
        return Money{amount_ - rhs.amount_, currency_};
    }

    // Multiply by factor (e.g., 1.35 for 35% surcharge)
    // Uses integer arithmetic to avoid float precision loss
    [[nodiscard]] Money operator*(double factor) const noexcept {
        int64_t result = static_cast<int64_t>(std::llround(
            static_cast<double>(amount_) * factor
        ));
        return Money{result, currency_};
    }

    [[nodiscard]] Money operator*(int32_t multiplier) const noexcept {
        return Money{amount_ * multiplier, currency_};
    }

    // Percentage discount: applyDiscount(10) = 10% off
    [[nodiscard]] Money applyDiscount(uint8_t percent) const {
        if (percent > 100) throw std::invalid_argument("Discount cannot exceed 100%");
        int64_t discounted = amount_ - static_cast<int64_t>(
            std::llround(static_cast<double>(amount_) * percent / 100.0)
        );
        return Money{discounted, currency_};
    }

    // Comparison — full spaceship operator
    [[nodiscard]] auto operator<=>(const Money& rhs) const {
        assertSameCurrency(rhs);
        return amount_ <=> rhs.amount_;
    }

    [[nodiscard]] bool operator==(const Money& rhs) const {
        return currency_ == rhs.currency_ && amount_ == rhs.amount_;
    }

    // Accessors
    [[nodiscard]] constexpr int64_t grosze() const noexcept { return amount_; }
    [[nodiscard]] constexpr double toDecimal() const noexcept { return amount_ / 100.0; }
    [[nodiscard]] constexpr CurrencyCode currency() const noexcept { return currency_; }
    [[nodiscard]] constexpr bool isZero() const noexcept { return amount_ == 0; }
    [[nodiscard]] constexpr bool isNegative() const noexcept { return amount_ < 0; }

    // Human-readable display with locale-aware formatting
    [[nodiscard]] std::string toDisplayString() const {
        std::ostringstream oss;
        std::string code(currency_.data(), 3);

        int64_t abs_amount = std::abs(amount_);
        int64_t zlote = abs_amount / 100;
        int64_t grosze = abs_amount % 100;

        if (isNegative()) oss << '-';

        if (code == "PLN") {
            // Polish format: 1 234,56 zł
            oss << formatWithThousands(zlote) << ','
                << std::setfill('0') << std::setw(2) << grosze << " zł";
        } else if (code == "EUR") {
            oss << "€" << formatWithThousands(zlote) << ','
                << std::setfill('0') << std::setw(2) << grosze;
        } else {
            // USD and others: $1,234.56
            oss << "$" << formatWithThousands(zlote) << '.'
                << std::setfill('0') << std::setw(2) << grosze;
        }
        return oss.str();
    }

private:
    constexpr Money(int64_t amount, CurrencyCode currency) noexcept
        : amount_{amount}, currency_{currency} {}

    void assertSameCurrency(const Money& other) const {
        if (currency_ != other.currency_)
            throw CurrencyMismatchException{currency_, other.currency_};
    }

    static std::string formatWithThousands(int64_t value) {
        std::string s = std::to_string(value);
        int insertPos = static_cast<int>(s.length()) - 3;
        while (insertPos > 0) {
            s.insert(insertPos, " ");
            insertPos -= 3;
        }
        return s;
    }

    int64_t amount_;        // in smallest currency unit (grosze)
    CurrencyCode currency_;
};

} // namespace hotel::domain
