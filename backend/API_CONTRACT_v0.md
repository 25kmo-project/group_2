# Bank-ATM API – Contract v0

## 0. Metatiedot
- **Nimi:** Bank-ATM REST API
- **Versio:** v0
- **Base URL:** `localhost:3000`
- **Content-Type:** `application/json; charset=utf-8`
- **Auth:** JWT (`Authorization: Bearer <token>`)
- **Virhevastauksen muoto (yhtenäinen):**
```json
{
  "error": "ErrorName",
  "message": "Human readable message",
  "stack": "optional in non-production"
}
```

## 1. Statuskoodit

| Koodi | Selitys               |
| ----- | --------------------- |
| 200   | OK                    |
| 201   | Created               |
| 204   | No Content            |
| 400   | Bad Request           |
| 401   | Unauthorized          |
| 403   | Forbidden             |
| 404   | Not Found             |
| 409   | Conflict              |
| 500   | Internal Server Error |

## 2. Tietomallit

### User

```json
{
  "idUser": "TESTUSER1",
  "firstName": "Testi",
  "lastName": "Asiakas",
  "streetAddress": "Testikatu 1"
}
```

### Card

```json
{
  "idCard": "CARD123456",
  "idUser": "TESTUSER1",
  "isLocked": false
}
```
> PIN:tä ei koskaan palauteta API:sta. PIN tallennetaan hashattuna.

### Account

```json
{
  "idAccount": 1,
  "idUser": "TESTUSER1",
  "balance": 500.00,
  "creditLimit": 0.00,
  "type": "debit"
}
```

### LogEntry

```json
{
  "idLog": 10,
  "idAccount": 1,
  "time": "2026-01-16T09:30:00.123Z",
  "balanceChange": -40.00
}
```

## 3. Autentikointi

### 3.1 Kirjautuminen (kortti + PIN)

#### POST `/auth/login`

```json
{
  "idCard": "CARD123456",
  "pin": "1234"
}
```

#### 200 OK

```json
{
  "token": "jwt...",
  "card": {
    "idCard": "CARD123456",
    "idUser": "TESTUSER1",
    "isLocked": false
  },
  "accounts": [
    { "idAccount": 1, "type": "debit", "balance": 500.00, "creditLimit": 0.00 },
    { "idAccount": 2, "type": "credit", "balance": 100.00, "creditLimit": 2000.00 }
  ],
  "requiresAccountSelection": true
}
```

### 3.2 Tilin valinta (kaksoiskortti)

#### POST `/auth/select-account`

```json
{
  "idAccount": 1
}
```

#### 200 OK

```json
{
  "activeAccount": {
    "idAccount": 1,
    "type": "debit"
  }
}
```

### 3.3 Logout

#### POST `/auth/logout` → 204 No Content

## 4. ATM-toiminnot

### 4.1 Saldon haku

#### GET `/accounts/{idAccount}`

```json
{
  "idAccount": 1,
  "balance": 480.00,
  "creditLimit": 0.00,
  "type": "debit"
}
```

### 4.2 Tilitapahtumat

#### GET `/accounts/{idAccount}/logs?limit=20&cursor=10`

```json
{
  "items": [
    {
      "idLog": 12,
      "time": "2026-01-16T09:30:00.123Z",
      "balanceChange": -20.00
    }
  ],
  "nextCursor": 12
}
```

### 4.3 Nosto (debit)

#### POST /accounts/{idAccount}/withdraw

```json
{ "amount": 40.00 }
```

#### 200 OK

```json
{
  "idAccount": 1,
  "balance": 440.00,
  "logged": true
}
```

### 4.4 Talletus

#### POST `/accounts/{idAccount}/deposit`

```json
{ "amount": 100.00 }
```

### 4.5 Tilisiirto

#### POST `/accounts/transfer`

```json
{
  "fromIdAccount": 1,
  "toIdAccount": 2,
  "amount": 75.00
}
```

## 5. Credit-toiminnot

### 5.1 Credit nosto

#### POST `/accounts/{idAccount}/credit/withdraw`

```json
{ "amount": 200.00 }
```

### 5.2 Credit takaisinmaksu

#### POST `/accounts/{idAccount}/credit/repay`

```json
{ "amount": 150.00 }
```

## 6. Kortit

### Lukitse kortti

#### POST `/cards/{idCard}/lock` → 204

### Avaa lukitus

#### POST `/cards/{idCard}/unlock` → 204

### Luo kortti

#### POST `/cards`

```json
{
  "idCard": "CARD999",
  "idUser": "TESTUSER1",
  "pin": "1234"
}
```

### Linkitä kortti tiliin

#### POST `/cards/{idCard}/accounts`

```json
{ "idAccount": 2 }
```

## 7. Käyttäjät (CRUD)

### Luo käyttäjä

#### POST `/users`

```json
{
  "idUser": "user123",
  "firstName": "Matti",
  "lastName": "Meikäläinen",
  "streetAddress": "Meikatie 1"
}
```

### Hae käyttäjä

#### GET `/users/{idUser}`

### Päivitä käyttäjä

#### PATCH `/users/{idUser}`

```json
{ "streetAddress": "Uusi osoite 5" }
```

### Poista käyttäjä

#### DELETE `/users/{idUser}` → 204

## 8. Tilit (CRUD)

### Luo tili

#### POST `/accounts`

```json
{
  "idUser": "user123",
  "balance": 1000.00,
  "creditLimit": 500.00
}
```

### Poista tili

#### DELETE `/accounts/{idAccount}` → 204

## 9. Lokit

### Hae yksittäinen loki

#### GET `/logs/{idLog}`

### Poista loki (admin)

#### DELETE `/logs/{idLog}` → 204

## 10. Huomioita

- Saldoa saa muuttaa vain transaktio-endpointeilla
- Kaikki nostot/talletukset luovat logimerkinnän
- Kaksoiskortti vaatii tilin valinnan
- PIN käsitellään vain hashattuna
