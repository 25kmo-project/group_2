# Group 2 Project

## Sisällysluettelo

- [Backend-osio](#backend-osio)
  - [API Käynnistys](#api-käynnistys)
  - [Autentikointi](#autentikointi)
  - [REST API Endpoints](#rest-api-endpoints)
    - [ATM-toiminnot (User)](#atm-toiminnot-user)
    - [Käyttäjät (Admin)](#käyttäjät-admin)
    - [Kortit (Admin)](#kortit-admin)
    - [Tilit (Admin)](#tilit-admin)
    - [Kortit ja Tilit (Admin)](#kortit-ja-tilit-admin)
    - [Logit (Admin)](#logit-admin)
  - [Backend status taulukko](#backend-status-taulukko)
  - [Role-järjestelmä (Admin vs User)](#role-järjestelmä-admin-vs-user)
  - [Ympäristömuuttujat (.env)](#ympäristömuuttujat-env)
  - [Database](#database)
  - [Tietokanta proseduurit](#tietokanta-proseduurit)
- [Qt Widget (Frontend)](#qt-widget-frontend)

## Dokumentaatio

- **[API_CONTRACT_v0.md](backend/API_CONTRACT_v0.md)** - Kattava API-dokumentaatio ja tekninen sopimus
- **[SETUP_AUTOSTART.md](SETUP_AUTOSTART.md)** - PM2 automaattikäynnistys ja tuotantoasennus

## Backend-osio

### API Käynnistys

```bash
cd backend
npm install
npm start
```

API pyörii osoitteessa: `http://localhost:3000`

### Autentikointi

#### Kirjautuminen (Login)

- **POST /auth/login** - Kirjaudu sisään kortilla ja PIN-koodilla
  ```json
  {
    "idCard": "CARD123456",
    "pin": "1234"
  }
  ```
  **Vastaus (200 OK):**
  ```json
  {
    "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "card": {
      "idCard": "CARD123456",
      "idUser": "TESTUSER1",
      "isLocked": false
    },
    "accounts": [
      {
        "idAccount": 14,
        "type": "debit",
        "balance": 500.00,
        "creditLimit": 0.00
      },
      {
        "idAccount": 15,
        "type": "credit",
        "balance": 0.00,
        "creditLimit": 2000.00
      }
    ],
    "requiresAccountSelection": true
  }
  ```

#### Uloskirjautuminen (Logout)

- **POST /auth/logout** - Kirjaudu ulos (token blacklistille)
  - Vaatii: `Authorization: Bearer <token>` header

#### Token käyttö

Kaikki suojatut endpointit vaativat JWT-tokenin headerissa:
```
Authorization: Bearer <token>
```

### REST API Endpoints

#### ATM-toiminnot (User)

**Huom:** Vaativat JWT-tokenin ja omistajuuden tiliin

- **GET /atm/:id** - Hae tilin saldo
  ```json
  {
    "idAccount": 14,
    "idUser": "TESTUSER1",
    "balance": 500.00,
    "creditLimit": 0.00
  }
  ```

- **GET /atm/:id/logs** - Hae tilin tapahtumahistoria
  ```json
  {
    "items": [
      {
        "idLog": 12,
        "time": "2026-01-16T09:30:00.123000",
        "balanceChange": -20.00
      }
    ]
  }
  ```

- **POST /atm/:id/withdraw** - Nosta rahaa (debit-tili)
  ```json
  {
    "amount": 40.00
  }
  ```
  **Vastaus:**
  ```json
  {
    "idAccount": 14,
    "balance": 460.00,
    "logged": true
  }
  ```

- **POST /atm/:id/credit/withdraw** - Nosta rahaa (credit-tili)
  ```json
  {
    "amount": 200.00
  }
  ```
  **Vastaus:**
  ```json
  {
    "idAccount": 15,
    "balance": -200.00,
    "logged": true
  }
  ```

#### Käyttäjät (Admin)

**Huom:** Vaativat JWT-tokenin ja `role: 'admin'`

- **GET /users/:idUser** - Hae käyttäjä ID:llä
  **Vastaus (200 OK)**
  ```json
  {
    "idUser": "USER123",
    "firstName": "Matti",
    "lastName": "Meikäläinen",
    "streetAddress": "Esimerkkitie 1",
    "role": "user"
  }
  ```

- **POST /users** - Luo uusi käyttäjä
  ```json
  {
    "idUser": "USER123",
    "firstName": "Matti",
    "lastName": "Meikäläinen",
    "streetAddress": "Esimerkkitie 1",
    "role": "user"
  }
  ```
  **Huom:** `role` kenttä on valinnainen (oletusarvo: "user"). Vaihtoehdot: "user" tai "admin".
  
  **Vastaus (201 Created)**
  ```json
  {
    "idUser": "USER123",
    "firstName": "Matti",
    "lastName": "Meikäläinen",
    "streetAddress": "Esimerkkitie 1",
    "role": "user"
  }
  ```

- **PUT /users/:idUser** - Päivitä käyttäjän tiedot
  ```json
  {
    "firstName": "Maija",
    "lastName": "Virtanen",
    "streetAddress": "Uusitie 5"
  }
  ```
  **Vastaus (200 OK)**
  ```json
  {
    "idUser": "USER123",
    "firstName": "Maija",
    "lastName": "Virtanen",
    "streetAddress": "Uusitie 5"
  }
  ```

- **DELETE /users/:idUser** - Poista käyttäjä
  **Vastaus (204 No Content)**

#### Kortit (Admin)

**Huom:** Vaativat JWT-tokenin ja `role: 'admin'`

- **GET /cards** - Hae kaikki kortit
  **Vastaus (200 OK)**
  ```json
  [
    {
      "idCard": "CARD123456",
      "idUser": "TESTUSER1",
      "isLocked": false
    },
    {
      "idCard": "ADMINCARD",
      "idUser": "ADMIN",
      "isLocked": false
    }
  ]
  ```

- **GET /cards/:idCard** - Hae kortti ID:llä
  **Vastaus (200 OK)**
  ```json
  {
    "idCard": "CARD123456",
    "idUser": "TESTUSER1",
    "isLocked": false
  }
  ```

- **POST /cards** - Luo uusi kortti (PIN hashataan bcryptillä)
  ```json
  {
    "idCard": "CARD789012",
    "idUser": "USER123",
    "cardPIN": "1234"
  }
  ```
  **Vastaus (201 Created)**
  ```json
  {
    "idCard": "CARD789012",
    "idUser": "USER123",
    "isLocked": false
  }
  ```

- **PUT /cards/:idCard/pin** - Päivitä kortin PIN
  ```json
  {
    "cardPIN": "5678"
  }
  ```
  **Vastaus (200 OK)**
  ```json
  {
    "message": "PIN updated successfully"
  }
  ```

- **POST /cards/:idCard/lock** - Lukitse kortti
  **Vastaus: (204 No Content)**

- **POST /cards/:idCard/unlock** - Avaa kortin lukitus
  **Vastaus: (204 No Content)**

- **DELETE /cards/:idCard** - Poista kortti
  **Vastaus: (204 No Content)**

#### Tilit (Admin)

**Huom:** Vaativat JWT-tokenin ja `role: 'admin'`

- **GET /accounts/:id** - Hae tilin tiedot
  **Vastaus (200 OK)**
  ```json
  {
    "idAccount": 14,
    "idUser": "TESTUSER1",
    "balance": 500.00,
    "creditLimit": 0.00
  }
  ```

- **POST /accounts** - Luo uusi tili
  ```json
  {
    "idUser": "TESTUSER1",
    "balance": 1000.00,
    "creditLimit": 500.00
  }
  ```
  **Vastaus (201 Created)**
  ```json
  {
    "idAccount": 16,
    "idUser": "TESTUSER1",
    "balance": 1000.00,
    "creditLimit": 500.00
  }
  ```

- **PUT /accounts/:id** - Päivitä tilin credit limit
  ```json
  {
    "creditLimit": 3000.00
  }
  ```
  **Vastaus (200 OK)**
  ```json
  {
    "message": "Credit limit updated successfully"
  }
  ```

- **DELETE /accounts/:id** - Poista tili
  **Vastaus: 204 No Content**
  
  **Huom:** Tili ei voi olla linkitettynä kortteihin

#### Kortit ja Tilit (Admin)

**Huom:** Vaativat JWT-tokenin ja `role: 'admin'`

- **GET /cardaccount/:idCard** - Hae kortin linkitetyt tilit
  **Vastaus (200 OK)**
  ```json
  [
    {
      "idCard": "CARD123456",
      "idAccount": 14
    },
    {
      "idCard": "CARD123456",
      "idAccount": 15
    }
  ]
  ```

- **POST /cardaccount** - Linkitä kortti tiliin
  ```json
  {
    "idCard": "CARD123456",
    "idAccount": 1
  }
  ```
  **Vastaus (201 Created)**
  ```json
  {
    "message": "Card linked to account successfully"
  }
  ```

- **PUT /cardaccount/:idCard** - Päivitä kortin tiliyhdistelmä
  ```json
  {
    "IdAccount": 1,
    "newIdAccount": 2
  }
  ```
  **Vastaus (200 OK)**
  ```json
  {
    "message": "Card account link updated successfully"
  }
  ```

- **DELETE /cardaccount/:idCard** - Poista kortin ja tilin linkki
  ```json
  {
    "IdAccount": 1
  }
  ```
  **Vastaus: (204 No Content)**

#### Logit (Admin)

**Huom:** Vaativat JWT-tokenin ja `role: 'admin'`

- **GET /log/:idAccount** - Hae tilin tapahtumalogit
  - Palauttaa kaikki tilin tapahtumat uusimmasta vanhimpaan
  - Esimerkki: `GET /log/1`
  - Vastaus:
  ```json
  [
    {
      "idLog": 10,
      "idAccount": 1,
      "time": "2026-01-16T09:30:00.123000",
      "balanceChange": -40.00
    },
    {
      "idLog": 9,
      "idAccount": 1,
      "time": "2026-01-15T14:20:00.000000",
      "balanceChange": 100.00
    }
  ]
  ```

### Backend status taulukko

| Tilanne                                | Status                        |
| -------------------------------------- | ----------------------------- |
| GET onnistuu                           | **200 OK**                    |
| POST luo resurssin                     | **201 Created**               |
| DELETE onnistuu                        | **204 No Content**            |
| Puuttuva / virheellinen data           | **400 Bad Request**           |
| Ei kirjautunut                         | **401 Unauthorized**          |
| Ei oikeuksia                           | **403 Forbidden**             |
| Resurssia ei ole                       | **404 Not Found**             |
| Duplikaatti (esim. kortti jo olemassa) | **409 Conflict**              |
| Odottamaton virhe                      | **500 Internal Server Error** |

### Role-järjestelmä (Admin vs User)

Järjestelmässä on kaksi käyttäjäroolia:
- **user** (oletus) - Tavallinen käyttäjä, pääsee `/atm/*` endpointeihin omilla tileillään
- **admin** - Admin-käyttäjä, pääsee kaikkiin endpointeihin (`/users/*`, `/cards/*`, `/accounts/*`, `/log/*`, `/cardaccount/*`)

**Admin-endpointit** (vaativat `role: 'admin'`):
- `GET/POST/PUT/DELETE /users/*` - Käyttäjien hallinta
- `GET/POST/PUT/DELETE /cards/*` - Korttien hallinta  
- `GET/POST/PUT/DELETE /accounts/*` - Tilien hallinta
- `GET/POST/PUT/DELETE /cardaccount/*` - Kortti-tili linkkien hallinta
- `GET /log/*` - Lokien katselu

**User-endpointit** (vaativat vain kirjautumisen ja omistajuuden):
- `GET/POST /atm/*` - Omat tilit ja transaktiot

**Testitunnukset:**
- **Tavallinen käyttäjä**: Kortti: `CARD123456`, PIN: `1234`
  - Debit-tili (ID: 14): 500.00 €
  - Credit-tili (ID: 15): 2000.00 € luottoraja
- **Admin-käyttäjä**: Kortti: `ADMINCARD`, PIN: `admin123`

**Admin-käyttäjän luominen tietokantaan:**
```sql
-- Luo admin-käyttäjä
CALL sp_create_user('admin1', 'Admin', 'User', 'Admin Street', 'admin');



### Ympäristömuuttujat (.env)

Luo `backend/.env` tiedosto:
```env
# Tietokanta
DB_HOST=localhost
DB_USER=root
DB_PASSWORD=your_password
DB_NAME=bank_db
DB_PORT=3306

# JWT
JWT_SECRET=your-secret-key-here

# PIN Pepper (lisäturvallisuus)
PIN_PEPPER=your-pepper-here

# Port
PORT=3000
```

### Database

**Skeeman ja proseduurien alustus:**
```bash
cd backend/db
sudo mysql -u root bank_db < schema.sql
sudo mysql -u root bank_db < procedures.sql
sudo mysql -u root bank_db < seed.sql
```

**Huom:** `seed.sql` sisältää testidatan (TESTUSER1 ja ADMIN käyttäjät)

### Tietokanta proseduurit

#### 1) Käyttäjät
#### 2) Tilit
#### 3) Kortit
#### 4) Transaktiot
#### 5) Luottotili
#### 6) Tapahtumalogit

#### Käyttäjien hallinta

**Käyttäjän lisäys**
```sql
-- Lisää uuden käyttäjän (role valinnainen, oletusarvo 'user')
CALL sp_create_user(iduser, fname, lname, streetaddress, role);
-- Esimerkki: CALL sp_create_user('user123', 'Matti', 'Meikäläinen', 'Meikatie 1', 'user');
-- Admin: CALL sp_create_user('admin1', 'Admin', 'User', 'Admin St', 'admin');
```

**Käyttäjän poisto**
```sql
-- Poistaa käyttäjän (käyttäjällä ei saa olla tilejä)
CALL sp_delete_user(iduser);
-- Esimerkki: CALL sp_delete_user('user123');
```

**Käyttäjätietojen lukeminen**
```sql
-- Hakee käyttäjän tiedot (mukaan lukien role)
CALL sp_read_user_info(iduser);
-- Esimerkki: CALL sp_read_user_info('user123');
-- Palauttaa: iduser, fname, lname, streetaddress, role
```

**Käyttäjätietojen päivittäminen**
```sql
-- Päivittää käyttäjän tiedot
CALL sp_update_user_info(iduser, fname, lname, streetaddress);
-- Esimerkki: CALL sp_update_user_info('user123', 'Maija', 'Meikäläinen', 'Uusitie 2');
```

#### Tilien hallinta

**Tilin lisäys**
```sql
-- Lisää uuden tilin käyttäjälle
CALL sp_add_account(iduser, balance, credit_limit);
-- Esimerkki: CALL sp_add_account('user123', 1000.00, 500.00);
```

**Tilin poisto**
```sql
-- Poistaa tilin (tilillä ei saa olla kortteja, logit poistetaan automaattisesti)
CALL sp_delete_account(idaccount);
-- Esimerkki: CALL sp_delete_account(1);
```

**Tilin tietojen lukeminen**
```sql
-- Hakee tilin tiedot
CALL sp_read_account_info(idaccount);
-- Esimerkki: CALL sp_read_account_info(1);
-- Palauttaa: idaccount, iduser, balance, creditlimit
```

#### Korttien hallinta

**Kortin luominen**
```sql
-- Luo uuden kortin (PIN on jo hashattu backendissä)
CALL sp_create_card(idcard, iduser, hashed_pin);
-- Esimerkki: CALL sp_create_card('CARD789012', 'user123', '$2b$10$...');
```

**Kortin tietojen lukeminen**
```sql
-- Hakee kortin tiedot (mukaan lukien cardPIN autentikoinnissa)
CALL sp_read_card_info(idcard);
-- Esimerkki: CALL sp_read_card_info('CARD123456');
-- Palauttaa: idcard, cardPIN, iduser, is_locked
```

**Kaikkien korttien lukeminen**
```sql
-- Hakee kaikki kortit (ilman PIN:ejä)
CALL sp_read_all_cards();
```

**Kortin PIN:in päivitys**
```sql
-- Päivittää kortin PIN:in (PIN on jo hashattu backendissä)
CALL sp_update_card_pin(idcard, new_hashed_pin);
-- Esimerkki: CALL sp_update_card_pin('CARD123456', '$2b$10$...');
```

**Kortin linkitettyjen tilien hakeminen**
```sql
-- Hakee kortin linkitetyt tilit
CALL sp_get_card_info(idcard);
-- Esimerkki: CALL sp_get_card_info('CARD123456');
-- Palauttaa: idcard, idaccount
```

**Kortin linkitys tiliin**
```sql
-- Linkittää kortin tiliin
CALL sp_card_to_account(idcard, idaccount);
-- Esimerkki: CALL sp_card_to_account('CARD123456', 1);
```

**Kortin poisto**
```sql
-- Poistaa kortin
CALL sp_delete_card(idcard);
-- Esimerkki: CALL sp_delete_card('CARD123456');
```

**Kortin ja tilin linkin poisto**
```sql
-- Poistaa kortin ja tilin välisen linkin
CALL sp_remove_card_from_account(idcard, idaccount);
-- Esimerkki: CALL sp_remove_card_from_account('CARD123456', 1);
```

**Kortin tiliyhdistelmän päivitys**
```sql
-- Päivittää kortin vanhasta tilistä uuteen tiliin
CALL sp_update_card_linked_account(idcard, old_idaccount, new_idaccount);
-- Esimerkki: CALL sp_update_card_linked_account('CARD123456', 1, 2);
```

**Kortin lukitseminen**
```sql
-- Lukitsee kortin ID:n perusteella
CALL sp_card_lock(idcard);
-- Esimerkki: CALL sp_card_lock('CARD123456');
```

**Kortin lukituksen poisto**
```sql
-- Poistaa kortin lukituksen ID:n perusteella
CALL sp_card_unlock(idcard);
-- Esimerkki: CALL sp_card_unlock('CARD123456');
```

#### Tilin transaktiot

**Talletus**
```sql
-- Tallettaa rahaa tilille
CALL sp_deposit(idaccount, amount);
-- Esimerkki: CALL sp_deposit(1, 100.00);
```

**Nosto**
```sql
-- Nostaa rahaa tililtä
CALL sp_withdraw(idaccount, amount);
-- Esimerkki: CALL sp_withdraw(1, 50.00);
```

**Tilisiirto**
```sql
-- Siirtää rahaa tililtä toiselle
CALL sp_transfer(idaccount_from, idaccount_to, amount);
-- Esimerkki: CALL sp_transfer(1, 2, 75.00);
```

#### Luottotili

**Luoton nosto**
```sql
-- Nostaa rahaa luottotililtä
CALL sp_credit_withdraw(idaccount, amount);
-- Esimerkki: CALL sp_credit_withdraw(1, 200.00);
```

**Luoton takaisinmaksu**
```sql
-- Maksaa rahaa takaisin luottotilille
CALL sp_credit_repay(idaccount, amount);
-- Esimerkki: CALL sp_credit_repay(1, 150.00);
```

**Luottorajan päivitys**
```sql
-- Päivittää tilin luottorajan
CALL sp_update_creditlimit(idaccount, creditlimit);
-- Esimerkki: CALL sp_update_creditlimit(1, 2000.00);
```

#### Tapahtumalogit

**Tilin lokien haku**
```sql
-- Hakee tilin kaikki tapahtumalogit uusimmasta vanhimpaan
CALL sp_read_account_logs(idaccount);
-- Esimerkki: CALL sp_read_account_logs(1);
-- Palauttaa: idlog, idaccount, time, balancechange
```

## Qt Widget (Frontend)

### Käynnistys

```bash
cd bank-automat
mkdir build
cd build
cmake ..
make
./bank_automat
```

### Kirjautuminen

Sovellus pyytää kirjautumisessa:
1. **Korttinumero** (esim. `CARD123456`)
2. **PIN-koodi** (esim. `1234`)

Kirjautumisen jälkeen valitaan tili (jos käyttäjällä on useita tilejä).

### Testitunnukset

- **Tavallinen käyttäjä**: Kortti: `CARD123456`, PIN: `1234`
- **Admin-käyttäjä**: Kortti: `ADMINCARD`, PIN: `admin123`

### Ominaisuudet

- Korttinumeron ja PIN-koodin syöttö
- Tilin valinta (kaksoiskortti)
- Saldon tarkistus
- Nostot (debit ja credit)
- Tapahtumahistoria
- Automaattinen uloskirjautuminen (timeout)
