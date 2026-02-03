/* Tämä tiedosto on seed data: tyhjentää taulut ja lisää testidatan (user, debit+credit-tilit, kaksoiskortti, lokit) */

USE bank_db;

-- Varmempi uudelleenkäyttö
SET FOREIGN_KEY_CHECKS = 0;

DELETE FROM log;
DELETE FROM accounts_cards;
DELETE FROM cards;
DELETE FROM accounts;
DELETE FROM users;

SET FOREIGN_KEY_CHECKS = 1;

-- Admin-käyttäjä
INSERT INTO users (iduser, fname, lname, streetaddress, role)
VALUES ('ADMIN', 'Admin', 'User', 'Admin Street 1', 'admin');

-- Testikäyttäjä
INSERT INTO users (iduser, fname, lname, streetaddress, role)
VALUES ('TESTUSER1', 'Testi', 'Asiakas', 'Testikatu 1', 'user');

-- Debit-tili
INSERT INTO accounts (iduser, balance, creditlimit)
VALUES ('TESTUSER1', 500.00, 0.00);
SET @debit_account_id = LAST_INSERT_ID();

-- Credit-tili
INSERT INTO accounts (iduser, balance, creditlimit)
VALUES ('TESTUSER1', 0.00, 2000.00);
SET @credit_account_id = LAST_INSERT_ID();

-- Admin-kortti (PIN: admin123)
INSERT INTO cards (idcard, cardPIN, iduser, is_locked)
VALUES (
    'ADMINCARD',
    '$2b$10$I6vM3OpkhevRgFMdAksoLOCRuOWbwY7RAZYQ2HhpQL/dfacAwv0Xa',
    'ADMIN',
    0
);

-- Testikäyttäjän kortti (PIN: 1234), is_locked = 0
INSERT INTO cards (idcard, cardPIN, iduser, is_locked, pin_attempts)
VALUES (
    'CARD123456',
    '$2b$10$DMquyzBwPOXOLK.gY14gD.2/TmPGXrUagN2kbkyu81OOB/1MoCb9C',
    'TESTUSER1',
    0,
    0
);

-- Kaksoiskortti
INSERT INTO accounts_cards (idaccount, idcard)
VALUES
(@debit_account_id, 'CARD123456'),
(@credit_account_id, 'CARD123456');

-- Tapahtumia log-tauluun
INSERT INTO log (idaccount, time, balancechange)
VALUES
(@debit_account_id,  NOW(6) - INTERVAL 3 DAY,  500.00),
(@debit_account_id,  NOW(6) - INTERVAL 2 DAY,  -20.00),
(@debit_account_id,  NOW(6) - INTERVAL 1 DAY,  -40.00),
(@credit_account_id, NOW(6) - INTERVAL 1 DAY, -100.00);
