#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "Reservation.h"
#include "Room.h"
#include "Amenity.h"
#include "MealPlan.h"

class Hotel {
private:
    std::vector<Reservation*> reservations;
    std::vector<Room> rooms;
    std::vector<Amenity> amenities;
    std::map<MealPlan, double> pricelist;

public:
    // Konstruktor
    Hotel() = default;

    // Sprawdza dostępność pokoju w danym dniu
    bool isAvailable(int room, const std::string& date) {
        for (auto* r : reservations) {
            if (r->getRoomNumber() == room && r->getDate() == date)
                return false;
        }
        return true;
    }

    // Wypisuje dostępność wszystkich pokoi
    void checkAvailability() {
        std::cout << "Checking availability...\n";
        for (const auto& room : rooms) {
            std::cout << "Room " << room.getNumber()
                << " is available.\n";
        }
    }

    // Dodaje rezerwację
    void addReservation(Reservation* r) {
        reservations.push_back(r);
        std::cout << "Reservation added.\n";
    }

    // Odrzuca rezerwację
    void refuseReservation(const std::string& reason) {
        std::cout << "Reservation refused: " << reason << "\n";
    }

    // Dodaje zniżkę do rezerwacji
    void addDiscount(int resId, double d) {
        for (auto* r : reservations) {
            if (r->getId() == resId) {
                r->applyDiscount(d);
                std::cout << "Discount applied.\n";
                return;
            }
        }
        std::cout << "Reservation not found.\n";
    }

    // Wyświetla wszystkie rezerwacje
    void showReservations() {
        std::cout << "Reservations:\n";
        for (auto* r : reservations) {
            r->print();
        }
    }

    // Check-in
    void checkIn(int resId) {
        for (auto* r : reservations) {
            if (r->getId() == resId) {
                r->checkIn();
                std::cout << "Checked in.\n";
                return;
            }
        }
        std::cout << "Reservation not found.\n";
    }

    // Check-out
    void checkOut(int resId) {
        for (auto* r : reservations) {
            if (r->getId() == resId) {
                r->checkOut();
                std::cout << "Checked out.\n";
                return;
            }
        }
        std::cout << "Reservation not found.\n";
    }

    // Dodaje udogodnienie
    void addAmenity(const Amenity& a) {
        amenities.push_back(a);
    }

    // Oblicza całkowitą cenę rezerwacji
    double calculateTotalPrice(int resId) {
        for (auto* r : reservations) {
            if (r->getId() == resId) {
                double base = r->getBasePrice();
                double meal = pricelist[r->getMealPlan()];
                return base + meal;
            }
        }
        return 0.0;
    }

    // Generuje raport
    void generateReport() {
        std::cout << "Hotel Report:\n";
        std::cout << "Reservations: " << reservations.size() << "\n";
        std::cout << "Rooms: " << rooms.size() << "\n";
        std::cout << "Amenities: " << amenities.size() << "\n";
    }

    // Wyszukuje rezerwację po nazwisku
    Reservation* findReservation(const std::string& n) {
        for (auto* r : reservations) {
            if (r->getName() == n)
                return r;
        }
        return nullptr;
    }
};
