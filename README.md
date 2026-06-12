# G6_Hotel — System Rezerwacji Hotelowej

**Temat projektu:** System Rezerwacji Hotelowej (Podstawy programowania w C/C++, sem. II)  
**Nazwa hotelu:** Pod Gwiazdami  
**Repozytorium:** [grzegorzmajmurek-web/G6_Hotel](https://github.com/grzegorzmajmurek-web/G6_Hotel)

---

## Spis treści

1. [Opis projektu i funkcjonalności](#1-opis-projektu-i-funkcjonalności)
2. [Struktura projektu (pliki i klasy)](#2-struktura-projektu-pliki-i-klasy)
3. [Instrukcja kompilacji i uruchomienia](#3-instrukcja-kompilacji-i-uruchomienia)
4. [Autorzy i podział pracy](#4-autorzy-i-podział-pracy)
5. [Uwagi implementacyjne i znane problemy](#5-uwagi-implementacyjne-i-znane-problemy)
6. [Sprawozdanie z zajęć projektowych](#6-sprawozdanie-z-zajęć-projektowych)

---

## 1. Opis projektu i funkcjonalności

### 1.1 Cel projektu

G6_Hotel to konsolowa aplikacja symulująca pracę recepcji hotelowej. Program umożliwia zarządzanie katalogiem pokoi, tworzenie i anulowanie rezerwacji, dodawanie usług dodatkowych, stosowanie kodów rabatowych oraz przegląd kalendarza zajętości pokoi. Dane mogą być przechowywane w bazie **MySQL/MariaDB** (XAMPP) dzięki integracji z **MySQL Connector/C++**.

Aplikacja została zaprojektowana z wykorzystaniem obiektowości w C++: dziedziczenia, polimorfizmu dynamicznego, inteligentnych wskaźników (`std::unique_ptr`, `std::shared_ptr`), kontenerów STL oraz obsługi wyjątków.

### 1.2 Funkcjonalności (MVP)

#### System rezerwacji
- Tworzenie rezerwacji dla: pojedynczej osoby, pary, rodziny/grupy (3–4 gości)
- Walidacja dat (przyjazd < wyjazd) i pojemności pokoju
- Sprawdzanie dostępności pokoi w wybranym terminie
- Anulowanie rezerwacji po ID
- Przegląd aktywnych rezerwacji z podsumowaniem kosztów

#### Zarządzanie pokojami
- Katalog trzech typów pokoi (dziedziczenie od klasy `Room`):
  - **Standard** — 200 PLN/noc, 2 osoby (pokoje 101, 102)
  - **Deluxe** — 350 PLN/noc, 3 osoby (pokoje 201, 202)
  - **Suite** — 600 PLN/noc, 4 osoby (pokój 301)
- Filtrowanie dostępnych pokoi według liczby gości i terminu

#### Cennik sezonowy
- **Sezon niski** (I–III, XI): mnożnik ×1.0
- **Sezon średni** (IV–V, IX–X): mnożnik ×1.2
- **Sezon wysoki** (VI–VIII, XII): mnożnik ×1.5
- Cena pokoju liczona osobno za każdą dobę pobytu

#### Usługi dodatkowe (polimorfizm — klasa `Service`)
| Usługa | Opis | Cennik |
|--------|------|--------|
| **SPA** | Sesje w strefie wellness | 180 PLN / sesja |
| **Posiłki — Breakfast** | Śniadania | 45 PLN / osoba / noc |
| **Posiłki — Half Board** | Śniadanie + obiadokolacja | 90 PLN / osoba / noc |
| **Posiłki — Full Board** | Pełne wyżywienie | 140 PLN / osoba / noc |
| **Basen** | Wypożyczenie leżaka (+20 PLN/dzień/os.) i ręcznika (+10 PLN/dzień/os.) | według wyboru |

#### Kody rabatowe
| Kod | Warunek | Rabat |
|-----|---------|-------|
| `RODZINA` | min. 3 gości | 10% od sumy |
| `LATO` | dowolny pobyt | 50 PLN |
| `DLUGIPOBYT` | 3+ nocy / 7+ nocy / 14+ nocy | 5% / 10% / 20% |

Jedna rezerwacja = jeden aktywny kod rabatowy (możliwe usunięcie i ponowne zastosowanie).

#### Kalendarz pokoju
- Widok miesięczny zajętości wybranego pokoju
- Oznaczenie sezonu cenowego dla każdego dnia

#### Baza danych (MySQL)
- Tabele: `klienci`, `pokoje`, `rezerwacje`, `uslugi`
- Automatyczne zapisywanie rezerwacji, usług i kodów rabatowych
- Wczytywanie danych przy starcie aplikacji
- Tryb awaryjny bez bazy (pamięć RAM) — gdy brak połączenia z MySQL

### 1.3 Menu aplikacji

```
===== MENU =====
1. Lista pokoi
2. Nowa rezerwacja
3. Dodaj usluge do rezerwacji
4. Lista rezerwacji
5. Kalendarz pokoju (miesiac)
6. Anuluj rezerwacje
7. Zastosuj kod rabatowy
8. Usun kod rabatowy
0. Wyjscie
```

---

## 2. Struktura projektu (pliki i klasy)

```
G6_Hotel/
├── Hotel.cpp              # main(), menu, przepływy użytkownika
├── Hotel.h / HotelImpl.cpp # klasa Hotel — logika biznesowa
├── Room.h / Room.cpp       # Room (abstr.) → StandardRoom, DeluxeRoom, Suite
├── Service.h / Service.cpp # Service (abstr.) → SpaService, MealService, PoolService
├── PromoCodeService.h/.cpp # PromoCodeService (rabat jako usługa)
├── Reservation.h/.cpp      # rezerwacja, kalkulacja kosztów
├── SeasonalPricing.h/.cpp  # sezonowe mnożniki cen
├── Date.h / Date.cpp       # obsługa dat, operatory porównania i <<
├── Database.h / Database.cpp # warstwa MySQL (Connector/C++)
├── hotelBaza.sql           # schemat bazy danych
├── umlDiagram              # diagram klas UML (tekstowy)
├── README.md               # niniejsza dokumentacja
└── Hotel/                  # projekt Visual Studio (.vcxproj)
```

### Diagram klas (skrót)

- **Hotel** — agreguje pokoje (`shared_ptr<Room>`) i rezerwacje (`unique_ptr<Reservation>`)
- **Reservation** — komponuje usługi (`unique_ptr<Service>`), używa `SeasonalPricing`
- **Room** — polimorfizm: `GetBasePricePerNight()`, `GetType()`, `Describe()`
- **Service** — polimorfizm: `GetCost()`, `GetName()`, `ToRecord()` (serializacja do DB)
- **Database** — połączenie JDBC, CRUD rezerwacji i usług

Pełny diagram: plik `umlDiagram`.

---

## 3. Instrukcja kompilacji i uruchomienia

### 3.1 Wymagania

| Składnik | Wersja / uwagi |
|----------|----------------|
| Kompilator C++ | C++17 lub nowszy (projekt VS: C++20) |
| Visual Studio | 2019/2022/2025 z workload „Desktop development with C++" |
| XAMPP | MySQL/MariaDB uruchomiony (Apache nie jest wymagany) |
| MySQL Connector/C++ | 8.1.0 (paczka **debug winx64**, API JDBC) |

### 3.2 Konfiguracja bazy danych

1. Uruchom **XAMPP** → włącz moduł **MySQL**.
2. Otwórz **phpMyAdmin** (`http://localhost/phpmyadmin`).
3. Zaimportuj plik `hotelBaza.sql` (tworzy bazę `hotel` z przykładowymi danymi).

### 3.3 Konfiguracja MySQL Connector/C++ w Visual Studio

> **Ważne:** Używamy **starego API JDBC** (`mysql_driver.h`, `cppconn/`), nie X DevAPI (`mysqlcppconn8`).

**Konfiguracja:** `Debug` | `x64` (paczka debugowa wymaga trybu Debug)

| Ustawienie | Wartość |
|------------|---------|
| C/C++ → Additional Include Directories | `D:\mysql-connector-c++-8.1.0-winx64-debug\mysql-connector-c++-8.1.0-winx64\include\jdbc` |
| Linker → Additional Library Directories | `D:\mysql-connector-c++-8.1.0-winx64-debug\mysql-connector-c++-8.1.0-winx64\lib64\vs14` |
| Linker → Additional Dependencies | `mysqlcppconn.lib` (**nie** `mysqlcppconn8.lib`) |
| C/C++ → Runtime Library | `Multi-threaded Debug DLL (/MDd)` |

**DLL przy uruchomieniu:** skopiuj plik `mysqlcppconn-*-vs14.dll` z folderu `lib64\` do katalogu z `Hotel.exe` (np. `Hotel\x64\Debug\`).

### 3.4 Kompilacja w Visual Studio

1. Otwórz plik `Hotel/Hotel.vcxproj` (lub `.sln`, jeśli istnieje).
2. Ustaw konfigurację: **Debug** + **x64**.
3. Upewnij się, że w projekcie są wszystkie pliki `.cpp` i `.h` (w tym `Database.cpp`).
4. **Kompilacja → Przebuduj rozwiązanie** (Ctrl+Shift+B).
5. Uruchom: **Ctrl+F5** (bez debugowania).

### 3.5 Kompilacja z linii poleceń (bez bazy — opcjonalnie)

Jeśli chcesz zbudować wersję bez MySQL, usuń `Database.cpp` z kompilacji i wywołania DB w `main()`:

```bash
g++ -std=c++20 -Wall -o hotel.exe ^
    Hotel.cpp HotelImpl.cpp Room.cpp Service.cpp Reservation.cpp ^
    PromoCodeService.cpp SeasonalPricing.cpp Date.cpp
```

### 3.6 Uruchomienie

1. Upewnij się, że **MySQL w XAMPP działa**.
2. Uruchom `Hotel.exe`.
3. Przy starcie program próbuje połączyć się z bazą:
   - host: `127.0.0.1:3306`
   - użytkownik: `root`
   - hasło: *(puste — domyślnie XAMPP)*
   - baza: `hotel`
4. Jeśli połączenie się powiedzie, zobaczysz: `[DB] Polaczono z baza danych "hotel".`
5. Jeśli nie — program działa w trybie pamięciowym (dane nie są zapisywane).

Dane logowania można zmienić w `Hotel.cpp`, w funkcji `main()`:

```cpp
db.Connect("tcp://127.0.0.1:3306", "root", "", "hotel");
```

---

## 4. Autorzy i podział pracy

### 4.1 Skład zespołu

| Lp. | Imię i nazwisko | Rola w zespole |
|-----|-----------------|----------------|
| 1. | Grzegorz Majmurek | Lider / koordynator |
| 2. | Damian Odrowąż | Programista |
| 3. | Dominik Piwowarczyk | Programista |
| 4. | Karol Michalak | Programista |

### 4.2 Podział klas i modułów (do uzupełnienia)

Przypisz członków zespołu do poniższych modułów. Każdy moduł obejmuje pliki `.h` i `.cpp` oraz odpowiedzialność za dany fragment architektury.

| Moduł / klasa | Pliki | Zakres odpowiedzialności | Osoba odpowiedzialna |
|---------------|-------|--------------------------|----------------------|
| **A — Date** | `Date.h`, `Date.cpp` | Struktura daty, parsowanie `YYYY-MM-DD`, operatory porównania (`<`, `==`, …), `operator<<`, funkcje `DaysBetween`, `RangesOverlap` | Dominik Piwowarczyk |
| **B — SeasonalPricing** | `SeasonalPricing.h`, `SeasonalPricing.cpp` | Enum `Season`, mnożniki sezonowe, nazwy sezonów | Damian Odrowąż |
| **C — Room** | `Room.h`, `Room.cpp` | Klasa abstrakcyjna `Room`, dziedziczenie: `StandardRoom`, `DeluxeRoom`, `Suite`, fabryka `MakeRoom()`, `operator<<`, `operator==` | Dominik Piwowarczyk|
| **D — Service** | `Service.h`, `Service.cpp` | Klasa abstrakcyjna `Service`, podklasy: `SpaService`, `MealService`, `PoolService`, `ServiceRecord`, `MakeServiceFromRecord()` | Karol Michalak |
| **E — PromoCodeService** | `PromoCodeService.h`, `PromoCodeService.cpp` | Rabat jako usługa (ujemny koszt), walidacja kodu, `ToRecord()` | Karol Michalak |
| **F — Reservation** | `Reservation.h`, `Reservation.cpp` | Tworzenie rezerwacji, lista usług, kalkulacja kosztów (pokój + sezon + usługi), `operator<<`, obsługa kodów rabatowych | Damian Odrowąż |
| **G — Hotel** | `Hotel.h`, `HotelImpl.cpp` | Zarządzanie pokojami i rezerwacjami, dostępność, kalendarz, integracja z `Database`, `RestoreReservation()` | Grzegorz Majmurek |
| **H — Menu / main** | `Hotel.cpp` | Punkt wejścia `main()`, menu konsolowe, przepływy użytkownika (rezerwacja, usługi, kody rabatowe), obsługa wejścia | Grzegorz Majmurek |
| **I — Database** | `Database.h`, `Database.cpp`, `hotelBaza.sql` | Połączenie MySQL (Connector/C++ JDBC), wczytywanie/zapisywanie rezerwacji i usług, schemat bazy | Damian Odrowąż |
| **J — Dokumentacja** | `README.md`, `umlDiagram` | README, diagram UML, sprawozdanie | Damian Odrowąż |
---

## 5. Uwagi implementacyjne i znane problemy

### 5.1 Decyzje projektowe

- **Baza danych zamiast plików CSV/binarnych** — zamiast zapisu do pliku zastosowano MySQL/MariaDB (XAMPP). Trwałość danych jest realizowana przez bazę; przy obronie warto to krótko uzasadnić prowadzącemu.
- **API JDBC, nie X DevAPI** — kod używa klasycznego API (`mysql_driver.h`, `cppconn/`), kompatybilnego z portem **3306** (XAMPP). Biblioteka linkowana: `mysqlcppconn.lib`, nie `mysqlcppconn8.lib`.
- **Rabaty jako usługi** — `PromoCodeService` dziedziczy po `Service` i zwraca ujemny koszt, co upraszcza kalkulację sumy w `Reservation::CalculateTotal()`.
- **Typy pokoi** — w projekcie: Standard / Deluxe / Suite (adaptacja przykładu ze slajdów: Standard / Apartament / Prezydencki).

### 5.2 Zastosowane techniki C++ (wymagania EK4)

| Technika | Gdzie zastosowano |
|----------|-------------------|
| Dziedziczenie | `Room` → Standard/Deluxe/Suite; `Service` → Spa/Meal/Pool/Promo |
| Polimorfizm dynamiczny | `virtual`/`override`, `vector<unique_ptr<Service>>`, `shared_ptr<Room>` |
| STL | `vector`, `string`, `memory`, `algorithm`, `iostream` |
| Inteligentne wskaźniki | `unique_ptr` (rezerwacje, usługi), `shared_ptr` (pokoje) |
| Wyjątki | `try-catch`, `throw std::invalid_argument` (Date, Room, Service, Reservation) |
| Przeciążanie operatorów | `Date`: `<`, `==`, `<<`; `Room`: `==`, `<<`; `Reservation`: `<<` |
| Enkapsulacja | pola `private`/`protected`, gettery `const` |
| `#pragma once` | wszystkie pliki nagłówkowe |

### 5.3 Znane problemy i ograniczenia

| Problem | Opis | Obejście |
|---------|------|----------|
| Wymagana konfiguracja **Debug \| x64** | Paczka konektora to wersja debug winx64 | Buduj tylko w Debug + x64; skopiuj DLL obok `.exe` |
| Brak MySQL → tryb pamięciowy | Bez bazy dane giną po zamknięciu programu | Uruchom MySQL w XAMPP przed startem aplikacji |
| Jeden kod rabatowy na rezerwację | Celowe ograniczenie biznesowe | Usuń kod (opcja 8), potem dodaj nowy (opcja 7) |
| Brak logowania użytkownika | Poza zakresem MVP | — |

---

## 6. Sprawozdanie z zajęć projektowych

### Zajęcia 1 (P5-1) — Organizacja i projektowanie architektury

**Cel:** Sformowanie zespołu, wybór tematu, diagram UML, konfiguracja repozytorium.

**Wykonane:**
- [ ] Wybrano temat: **System Rezerwacji Hotelowej**
- [ ] Sformowano zespół (3–4 osoby) i wyznaczono lidera
- [ ] Utworzono repozytorium Git i plik README.md
- [ ] Zdefiniowano MVP (rezerwacje, pokoje, sezonowość, usługi, kody rabatowe)
- [ ] Przygotowano diagram klas UML (`umlDiagram`)

**Kamień milowy #1:** diagram UML zaakceptowany, podział zadań ustalony.

---

### Zajęcia 2 (P5-2) — Implementacja rdzenia

**Cel:** Przełożenie UML na pliki `.h`/`.cpp`, pierwsza wspólna kompilacja.

**Wykonane:**
- [ ] Utworzono szkielety klas: `Room`, `Service`, `Reservation`, `Hotel`, `Date`
- [ ] Zastosowano `#pragma once`, listy inicjalizacyjne, enkapsulację
- [ ] Scalono pracę zespołu w repozytorium Git
- [ ] Projekt kompiluje się bez błędów u każdego członka zespołu

**Kamień milowy #2:** działająca kompilacja, instancje klas tworzone w `main()`.

---

### Zajęcia 3 (P5-3) — Rozbudowa logiki i Code Review

**Cel:** Implementacja logiki biznesowej, peer review w Git.

**Wykonane:**
- [ ] Zaimplementowano kalkulację kosztów (`Reservation`, `SeasonalPricing`)
- [ ] Dodano walidację danych wejściowych (daty, liczba gości, kody rabatowe)
- [ ] Zaimplementowano sprawdzanie dostępności pokoi i kalendarz
- [ ] Przeprowadzono Code Review (Pull Requesty z komentarzami)

**Kamień milowy #3:** główna logika działa, repozytorium zawiera ślady review.

---

### Zajęcia 4 (P5-4) — Polimorfizm i testy integracyjne

**Cel:** Kolekcje polimorficzne, `virtual`/`override`, testy end-to-end.

**Wykonane:**
- [ ] `vector<unique_ptr<Service>>` — polimorficzne usługi (SPA, posiłki, basen, rabat)
- [ ] `shared_ptr<Room>` — polimorficzne pokoje (Standard, Deluxe, Suite)
- [ ] Wirtualne destruktory w klasach bazowych
- [ ] Test integracyjny: rezerwacja → usługa → kod rabatowy → podsumowanie kosztów
- [ ] Brak object slicing (wskaźniki, nie obiekty przez wartość)

**Kamień milowy #4:** polimorfizm zintegrowany, scenariusze end-to-end działają.

---

### Zajęcia 5 (P5-5) — Finalizacja i przygotowanie do prezentacji

**Cel:** Testowanie, obsługa błędów, przeciążanie operatorów, stabilność.

**Wykonane:**
- [ ] Testy brzegowe (puste dane, złe daty, brak pokoi, nieznany kod rabatowy)
- [ ] Obsługa wyjątków (`try-catch` w `main`, `Date::Parse`, konstruktory klas)
- [ ] Przeciążanie operatorów: `operator<<` (Date, Room, Reservation), `operator==` (Room, Date)
- [ ] Integracja z bazą MySQL (`Database`, `hotelBaza.sql`)
- [ ] Przygotowano scenariusz prezentacji (demo + omówienie kodu)

**Kamień milowy #5:** aplikacja w wersji Release Candidate, gotowa do obrony.

---

### Podsumowanie projektu

| Aspekt | Status |
|--------|--------|
| Funkcjonalność MVP | ✅ Zrealizowana |
| Dziedziczenie i polimorfizm | ✅ Room, Service |
| STL i inteligentne wskaźniki | ✅ |
| Obsługa wyjątków | ✅ |
| Przeciążanie operatorów | ✅ |
| Baza danych MySQL | ✅ |
| Diagram UML | ✅ (`umlDiagram`) |
| Dokumentacja README | ✅ |
---

*Ostatnia aktualizacja README: czerwiec 2026*
