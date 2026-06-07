# G6_Hotel

System zarządzania rezerwacjami hotelowymi umożliwiający obsługę klientów, pokoi, zniżek oraz dodatkowych usług oferowanych przez hotel.

---

## 1. Opis projektu

G6_Hotel symuluje działanie recepcji hotelowej. Po uruchomieniu program inicjuje hotel „Pod Gwiazdami" z pięcioma pokojami (101, 102, 201, 202, 301) i udostępnia interaktywne menu konsolowe, przez które można zarządzać całym cyklem życia rezerwacji.

## 2. Funkcjonalności
Funkcjonalności
Zarządzanie pokojami

Przeglądanie katalogu pokoi trzech typów: Standard (200 zł/noc, 2 osoby), Deluxe (350 zł/noc, 3 osoby), Suite (600 zł/noc, 4 osoby)
Sprawdzanie dostępności pokoi w wybranym terminie
Filtrowanie pokoi według liczby gości

Rezerwacje

Tworzenie rezerwacji dla pojedynczej osoby, pary lub rodziny/grupy (do 4 osób)
Walidacja dat (przyjazd musi być wcześniejszy niż wyjazd) i pojemności pokoju
Anulowanie istniejącej rezerwacji po ID
Przegląd wszystkich aktywnych rezerwacji z podsumowaniem kosztów

Usługi dodatkowe

Dodawanie SPA do rezerwacji (180 zł za sesję)
Dodawanie wyżywienia w trzech wariantach: śniadania (Breakfast), śniadania i obiadokolacje (Half Board), pełne wyżywienie (Full Board)

Kody rabatowe

RODZINA – 10% zniżki dla rezerwacji na min. 3 osoby
LATO – stały rabat 50 zł
DLUGIPOBYT – 5% (3+ noce), 10% (7+ noce), 20% (14+ noce)
Każda rezerwacja może mieć tylko jeden aktywny kod; możliwe jest jego usunięcie i zastąpienie nowym

Kalendarz

Wyświetlanie grafiku zajętości wybranego pokoju dla konkretnego miesiąca i roku

## 3. Struktura projektu

Plik                                                Opis
Hotel.cpp                                           Punkt wejścia (main), logika menu, przepływy interakcji
Hotel.h / HotelImpl.cpp                             Klasa Hotel — zarządzanie pokojami i rezerwacjami
Room.h / Room.cpp                                   Abstrakcyjna klasa Room i podklasy StandardRoom, DeluxeRoom, Suite
Service.h / Service.cpp                             Abstrakcyjna klasa Service i podklasy SpaService
Reservation.h / Reservation.cpp                     Klasa Reservation — przechowuje dane rezerwacji i oblicza koszt
PromoCodeService.h / PromoCodeService.cpp           Implementacja rabatów jako Service
ServiceSeasonalPricing.h / SeasonalPricing.cpp      Obsługa sezonowych zmian cen
Date.h / Date.cpp                                   Klasa Date — parsowanie, porównywanie i wyświetlanie dat
