-- phpMyAdmin SQL Dump
-- Baza danych dla systemu rezerwacji hotelowych "Pod Gwiazdami" (G6_Hotel)
--
-- Host: 127.0.0.1 (XAMPP / MariaDB)
-- Silnik: InnoDB, kodowanie: utf8mb4
--
-- Schemat dostosowany do kodu C++ (klasy Hotel, Room, Reservation, Service).
-- Tabele:
--   klienci    - dane gosci (imie + nazwisko)
--   pokoje     - katalog pokoi (typ, pojemnosc, cena bazowa za dobe)
--   rezerwacje - rezerwacje (gosc, pokoj, termin, liczba osob, cena, kod rabatowy)
--   uslugi     - uslugi dodatkowe przypisane do rezerwacji (SPA, posilki, basen, rabat)

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Baza danych: `hotel`
--
CREATE DATABASE IF NOT EXISTS `hotel` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `hotel`;

-- Usuwamy stare tabele (kolejnosc wazna ze wzgledu na klucze obce)
DROP TABLE IF EXISTS `uslugi`;
DROP TABLE IF EXISTS `rezerwacje`;
DROP TABLE IF EXISTS `pokoje`;
DROP TABLE IF EXISTS `klienci`;

-- --------------------------------------------------------

--
-- Struktura tabeli `klienci`
--
CREATE TABLE `klienci` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(100) NOT NULL,
  `lastname` VARCHAR(100) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Przykladowe dane `klienci`
--
INSERT INTO `klienci` (`id`, `name`, `lastname`) VALUES
(1, 'Jan', 'Kowalski'),
(2, 'Anna', 'Nowak'),
(3, 'Piotr', 'Wisniewski');

-- --------------------------------------------------------

--
-- Struktura tabeli `pokoje`
-- Katalog pokoi. Wartosci `type`, `capacity` i `price` odpowiadaja klasom
-- StandardRoom / DeluxeRoom / Suite w kodzie C++.
--
CREATE TABLE `pokoje` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `room_number` INT NOT NULL,
  `type` VARCHAR(50) NOT NULL,
  `capacity` INT NOT NULL,
  `price` DECIMAL(10,2) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_room_number` (`room_number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dane `pokoje` (cena bazowa za dobe, bez sezonowosci)
--
INSERT INTO `pokoje` (`id`, `room_number`, `type`, `capacity`, `price`) VALUES
(1, 101, 'Standard', 2, 200.00),
(2, 102, 'Standard', 2, 200.00),
(3, 201, 'Deluxe',   3, 350.00),
(4, 202, 'Deluxe',   3, 350.00),
(5, 301, 'Suite',    4, 600.00);

-- --------------------------------------------------------

--
-- Struktura tabeli `rezerwacje`
--   guests          - liczba gosci objetych rezerwacja
--   reservation_price - laczna cena (pokoj sezonowo + uslugi - rabaty)
--   promo_applied   - czy do rezerwacji przypisano kod rabatowy (0/1)
--
CREATE TABLE `rezerwacje` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `klient_id` INT DEFAULT NULL,
  `room_id` INT DEFAULT NULL,
  `room_number` INT DEFAULT NULL,
  `guests` INT NOT NULL DEFAULT 1,
  `reservation_price` DECIMAL(10,2) NOT NULL DEFAULT 0.00,
  `promo_applied` TINYINT(1) NOT NULL DEFAULT 0,
  `visiting_start` DATE NOT NULL,
  `visiting_end` DATE NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_rezerwacje_klient` (`klient_id`),
  KEY `fk_rezerwacje_pokoj` (`room_id`),
  CONSTRAINT `fk_rezerwacje_klient` FOREIGN KEY (`klient_id`) REFERENCES `klienci` (`id`) ON DELETE SET NULL,
  CONSTRAINT `fk_rezerwacje_pokoj` FOREIGN KEY (`room_id`) REFERENCES `pokoje` (`id`) ON DELETE SET NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Przykladowa rezerwacja (Suite 301 dla 4 osob)
--
INSERT INTO `rezerwacje`
  (`id`, `klient_id`, `room_id`, `room_number`, `guests`, `reservation_price`, `promo_applied`, `visiting_start`, `visiting_end`) VALUES
(1, 1, 5, 301, 4, 27000.00, 0, '2026-06-20', '2026-07-20');

-- --------------------------------------------------------

--
-- Struktura tabeli `uslugi`
-- Uslugi dodatkowe przypisane do rezerwacji. Pola parametrow zaleza od `service_type`:
--   SPA   -> spa_sessions
--   MEAL  -> meal_plan (BREAKFAST | HALFBOARD | FULLBOARD)
--   POOL  -> pool_days, pool_sunbed, pool_towel
--   PROMO -> promo_code, promo_discount
--
CREATE TABLE `uslugi` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `rezerwacja_id` INT NOT NULL,
  `service_type` ENUM('SPA','MEAL','POOL','PROMO') NOT NULL,
  `spa_sessions` INT DEFAULT NULL,
  `meal_plan` ENUM('BREAKFAST','HALFBOARD','FULLBOARD') DEFAULT NULL,
  `pool_days` INT DEFAULT NULL,
  `pool_sunbed` TINYINT(1) DEFAULT NULL,
  `pool_towel` TINYINT(1) DEFAULT NULL,
  `promo_code` VARCHAR(50) DEFAULT NULL,
  `promo_discount` DECIMAL(10,2) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_uslugi_rezerwacja` (`rezerwacja_id`),
  CONSTRAINT `fk_uslugi_rezerwacja` FOREIGN KEY (`rezerwacja_id`) REFERENCES `rezerwacje` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
