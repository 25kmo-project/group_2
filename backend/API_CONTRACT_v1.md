# API Contract v1 – Bank ATM REST API

## Yleistä

- **Versio:** v1
- **Base URL:** `localhost:3000`
- **Content-Type:** `application/json`
- **Autentikointi:** JWT (`Authorization: Bearer <token>`)
- **Tietokanta:** MySQL (InnoDB)
- **Aikamuodot:** MySQL `timestamp(6)`

### Yhtenäinen virhemuoto
```json
{
  "error": "Error",
  "message": "Human readable message",
  "stack": "Dev-only stacktrace"
}
```

## Autentikointi

### Login

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
  "token": "jwt-token",
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
    }
  ],
  "requiresAccountSelection": false
}
```

### Logout

#### POST `/auth/logout` → 204 No Content

## Users (Customers) – CRUD (Admin)

### Create user

#### POST `/users`

```json
{
  "idUser": "test123",
  "firstName": "Testi",
  "lastName": "Käyttäjä",
  "streetAddress": "Testikatu 1"
}
```

### Read user

#### GET `/users/{idUser}`

### Update user

#### PUT `/users/{idUser}`

```json
{
  "firstName": "Testi2",
  "lastName": "Käyttäjä2",
  "streetAddress": "Uusikatu 99"
}
```

### Delete user

#### DELETE `/users/{idUser}` → 204

## Accounts – CRUD (Admin)

### Create account

#### POST `/accounts`

```json
{
  "idUser": "test123",
  "balance": 1000.00,
  "creditLimit": 0.00
}
```

### Read account

#### GET `/accounts/{idAccount}`

### Update account

#### PUT `/accounts/{idAccount}`

```json
{
  "creditLimit": 500.00
}
```

### Delete account

#### DELETE `/accounts/{idAccount}` → 204 / 409

## Cards – CRUD (Admin)

> PIN tallennetaan hashattuna (`cardPIN`), eikä koskaan palauteta API:ssa.

### Create card

#### POST `/cards`

```json
{
  "idCard": "CARD999",
  "idUser": "test123",
  "pin": "1234"
}
```

### Read card

#### GET `/cards/{idCard}`

### Update card

#### PUT `/cards/{idCard}`

```json
{
  "isLocked": true,
  "pin": "0000"
}
```

### Delete card

#### DELETE `/cards/{idCard}` → 204 / 409

## Accounts ↔ Cards (Linkkitaulu) – CRUD

### Create link

#### POST `/accounts-cards`

```json
{
  "idAccount": 1,
  "idCard": "CARD999"
}
```

### Read all links

#### GET `/accounts-cards`

### Read by account

#### GET `/accounts-cards/account/{idAccount}`

### Read by card

#### GET `/accounts-cards/card/{idCard}`

### Delete link

#### DELETE `/accounts-cards`

```json
{
  "idAccount": 1,
  "idCard": "CARD999"
}
```

## Logs – CRUD (Admin / Debug)

### Create log

#### POST `/logs`

```json
{
  "idAccount": 1,
  "balanceChange": -20.00
}
```

### Read all logs

#### GET `/logs`

### Read log

#### GET `/logs/{idLog}`

### Update log

#### PUT `/logs/{idLog}`

```json
{
  "balanceChange": -25.00
}
```

### Delete log

#### DELETE `/logs/{idLog}` → 204

## ATM-toiminnot (User)

### Get balance

#### GET `/atm/balance`

### Withdraw

#### POST `/atm/withdraw`

```json
{
  "amount": 40.00
}
```

### Deposit

#### POST `/atm/deposit`

```json
{
  "amount": 100.00
}
```

### Transfer

#### POST `/atm/transfer`

```json
{
  "toAccount": 2,
  "amount": 75.00
}
```

## Roolit

- **User**
   - ATM-toiminnot
   - Omat tilit ja saldot
- **Admin**
   - Kaikki CRUD-endpointit
   - Käyttäjät, tilit, kortit, linkitykset, lokit

## Versiohistoria

- **v0** – Alkuperäinen API-sopimus
- **v1** – CRUD kaikille tauluille + autentikointi + ATM-toiminnot + roolit

## Testaus

Kaikki endpointit on testattu Postmanilla kokoelmassa:

### CRUD demo

- Users
- Accounts
- Cards
- Accounts_Cards
- Logs