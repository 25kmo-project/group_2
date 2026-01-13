/* Tämä tiedosto on seed-data SQL -skripti. Se tyhjentää tietokannan taulut ja lisää perusdatan (testikäyttäjä, debit- ja credit-tili, kortti ja 
tapahtumia), jotta backend/QT:ta voidaan testata heti */

-- Valitaan tietokanta
USE bank_db;

-- Tyhjennetään taulut "uudelleenkäyttöä varten"
DELETE FROM log;
DELETE FROM accounts_cards;
DELETE FROM cards;
DELETE FROM accounts;
DELETE FROM users;

-- Lisätään testikäyttäjä
INSERT INTO users (iduser, fname, lname, streetaddress)
VALUES ('TESTUSER1', 'Testi', 'Asiakas', 'Testikatu 1');

-- Luodaan kaksi tiliä: debit ja credit
-- Debit-tili
INSERT INTO accounts (iduser, balance, creditlimit)
VALUES ('TESTUSER1', 500.00, 0.00);
SET @debit_account_id = LAST_INSERT_ID();

-- Credit-tili
INSERT INTO accounts (iduser, balance, creditlimit)
VALUES ('TESTUSER1', 0.00, 2000.00);
SET @credit_account_id = LAST_INSERT_ID();

-- Lisätään kortti ja PIN-hash
INSERT INTO cards (idcard, cardPIN, iduser)
VALUES ('CARD123456', '$2b$10$raFg/F.nby0A9jB1VLCgJOZrrwjXq8igIxcrAyHed20Ev4pnmLwOS', 'TESTUSER1');

-- Liitetään kortti molempiin tileihin
INSERT INTO accounts_cards (idaccount, idcard)
VALUES
(@debit_account_id, 'CARD123456'),
(@credit_account_id, 'CARD123456');

-- Lisätään tapahtumia log -tauluun
INSERT INTO log (idaccount, time, balancechange)
VALUES
(@debit_account_id,  NOW(6) - INTERVAL 3 DAY,  500.00),
(@debit_account_id,  NOW(6) - INTERVAL 2 DAY,  -20.00),
(@debit_account_id,  NOW(6) - INTERVAL 1 DAY,  -40.00),
(@credit_account_id, NOW(6) - INTERVAL 1 DAY, -100.00);
