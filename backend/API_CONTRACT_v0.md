# API-sopimus v0 (Qt ↔ Backend)

Base URL (local): `http://localhost:3000`  
Kaikki pyynnöt ja vastaukset ovat JSON-muodossa.  
Autentikointi: `Authorization: Bearer <token>` (kirjautumisen jälkeen).

## Yhteinen virhemuoto
```json
{ "error": "Error", "message": "..." }
```

## Mallinnus: DEBIT vs CREDIT (tärkeä)

Backend päättelee tilin tyypin skeemasta:
- jos `creditlimit > 0` → `CREDIT`
- muuten → `DEBIT`

**DB-proseduurien mukainen luottotilin logiikka (CREDIT):**
- `balance`= **käytetty luotto / velka** (0…creditlimit), ei “tilin saldo”
- käytettävissä oleva luotto: `availableCredit = creditlimit - balance`
- luoton nosto kasvattaa `balance`a
- luoton takaisinmaksu pienentää `balance`a

**DEBIT-tilillä:**
- `balance` = rahasaldo, ei saa mennä miinukselle.

### 1) POST /auth/login

Kirjautuminen kortin ID:llä ja PIN-koodilla.<br>
PIN tarkistetaan bcryptillä.<br>
Jos kortilla on useampi tili (esim. debit + credit), Qt pyytää käyttäjältä tilin valinnan.

**Pyyntö**
```json
{ "cardId": "CARD123456", "pin": "1234" }
```

**Vastaus 200 OK**
```json
{
  "token": "token-here",
  "card": { "idcard": "CARD123456", "iduser": "TESTUSER1" },
  "accounts": [
    {
      "idaccount": 1,
      "type": "DEBIT",
      "balance": 500.00,
      "creditlimit": 0.00,
      "availableCredit": null
    },
    {
      "idaccount": 2,
      "type": "CREDIT",
      "balance": 300.00,
      "creditlimit": 2000.00,
      "availableCredit": 1700.00
    }
  ],
  "requiresAccountChoice": true
}
```

**Huomiot**
- `availableCredit` palautetaan vain CREDIT-tileille (muuten `null`).
- `requiresAccountChoice = true`, jos tilejä > 1.

**Statuskoodit**
- 200 OK – kirjautuminen onnistui
- 400 Bad Request – `cardId` tai `pin` puuttuu
- 401 Unauthorized – virheellinen kortti-ID tai PIN
- 423 Locked – kortti lukittu (kun toteutettu / DB `cards.islocked`)

### 2) GET /accounts/:id/balance

Palauttaa tilin saldon/velan ja luottorajan.

**Vastaus 200 OK**
```json
{
  "idaccount": 2,
  "type": "CREDIT",
  "balance": 300.00,
  "creditlimit": 2000.00,
  "availableCredit": 1700.00
}
```

**Statuskoodit**
- 200 OK
- 401 Unauthorized
- 403 Forbidden – ei oikeutta kyseiseen tiliin
- 404 Not Found – tiliä ei löydy

### 3) POST /accounts/:id/withdraw

Nosto tililtä.<br>
Summa on vapaavalintainen, mutta sen täytyy olla toteutettavissa **20 € ja 50 € seteleillä**.<br>
Backend hoitaa sekä DEBIT- että CREDIT-tilit:
- DEBIT → kutsuu `sp_withdraw(idaccount, amount)`
- CREDIT → kutsuu `sp_credit_withdraw(idaccount, amount)`

**Pyyntö**
```json
{ "amount": 130 }
```

**Vastaus 200 OK**
```json
{
  "idaccount": 1,
  "type": "DEBIT",
  "amount": 130,
  "dispensed": { "eur50": 1, "eur20": 4 },
  "balanceBefore": 480.00,
  "balanceAfter": 350.00,
  "logId": 123
}
```

**Validointisäännöt**
- `amount` on positiivinen kokonaisluku
- `amount` on muodostettavissa 20 € ja 50 € seteleistä<br>
(löytyy kokonaisluvut a,b siten että `20*a + 50*b = amount`)
- DEBIT: `balance >= amount`
- CREDIT (DB-proseduurien mukaisesti): `availableCredit = creditlimit - balance` ja `availableCredit >= amount`

**Statuskoodit**
- 200 OK – nosto onnistui
- 400 Bad Request – virheellinen summa / ei muodostettavissa seteleillä
- 401 Unauthorized
- 403 Forbidden
- 404 Not Found – tiliä ei löydy
- 409 Conflict – kate tai käytettävissä oleva luotto ei riitä

### 4) POST /accounts/:id/deposit

Talletus DEBIT-tilille (tai yleisesti tilille, jos sallitaan).<br>
DB-proseduuri: `sp_deposit(idaccount, amount)`.

**Pyyntö**
```json
{ "amount": 100.00 }
```

**Vastaus 200 OK**
```json
{
  "idaccount": 1,
  "type": "DEBIT",
  "amount": 100.00,
  "balanceBefore": 350.00,
  "balanceAfter": 450.00,
  "logId": 124
}
```

**Validointisäännöt**
- `amount` > 0 (desimaalit sallittu 2 tarkkuudella)

**Statuskoodit**
- 200 OK
- 400 Bad Request
- 401 Unauthorized
- 403 Forbidden
- 404 Not Found

### 5) POST /accounts/:id/credit-repay

Luottotilin takaisinmaksu.<br>
DB-proseduuri: `sp_credit_repay(idaccount, amount)`.

**Pyyntö**
```json
{ "amount": 150.00 }
```

**Vastaus**
```json
{
  "idaccount": 2,
  "type": "CREDIT",
  "amount": 150.00,
  "balanceBefore": 300.00,
  "balanceAfter": 150.00,
  "availableCreditAfter": 1850.00,
  "logId": 125
}
```

**Validointisäännöt (DB:n mukaisesti)**
- `amount` > 0
- ei saa maksaa “liikaa”: `balance >= amount` (balance = käytetty luotto)

**Statuskoodit**
- 200 OK
- 400 Bad Request
- 401 Unauthorized
- 403 Forbidden
- 404 Not Found
- 409 Conflict – yritettiin maksaa liikaa / muu ristiriita

### 6) POST /transfer

Tilisiirto tililtä toiselle.<br>
DB-proseduuri: `sp_transfer(idaccount_from, idaccount_to, amount)`.

**Pyyntö**
```json
{ "fromAccountId": 1, "toAccountId": 2, "amount": 75.00 }
```

**Vastaus 200 OK**
```json
{
  "fromAccountId": 1,
  "toAccountId": 2,
  "amount": 75.00,
  "fromBalanceBefore": 450.00,
  "fromBalanceAfter": 375.00,
  "toBalanceBefore": 150.00,
  "toBalanceAfter": 225.00,
  "fromLogId": 126,
  "toLogId": 127
}
```

**Validointisäännöt**
- `amount` > 0
- `fromAccountId != toAccountId`
- lähdetilillä oltava kate (DEBIT-logiikka DB-proseduurin mukaisesti)

**Statuskoodit**
- 200 OK
- 400 Bad Request
- 401 Unauthorized
- 403 Forbidden – ei oikeutta lähdetiliin (ja/tai kohdetiliin)
- 404 Not Found – lähde tai kohde puuttuu
- 409 Conflict – kate ei riitä

### 7) GET /accounts/:id/transactions

Tilitapahtumien selaus **10 tapahtumaa kerrallaan**, eteen ja taakse.

**Query-parametrit**
- `limit` (oletus 10, max 50)
- `cursor` (valinnainen): API:n palauttama osoitin
- `direction` = `next` | `prev` (oletus `next`)

**Esimerkki: ensimmäinen sivu (uusimmat)**
`GET /accounts/1/transactions?limit=10`

**Vastaus 200 OK**
```json
{
  "idaccount": 1,
  "limit": 10,
  "items": [
    { "idlog": 120, "time": "2026-01-14T10:00:00.000Z", "balancechange": -40.00 },
    { "idlog": 119, "time": "2026-01-13T10:00:00.000Z", "balancechange": -20.00 }
  ],
  "nextCursor": "opaque-cursor-for-older-items",
  "prevCursor": null
}
```

**Statuskoodit**
- 200 OK
- 401 Unauthorized
- 403 Forbidden
- 404 Not Found

### 8) POST /cards/:id/lock

DB-proseduuri: `sp_card_lock(idcard)`

**Vastaus 204 No Content (tai 200 OK)**
- 204 No Content – onnistui
- 401/403 – ei oikeutta
- 404 – korttia ei löydy

### 9) POST /cards/:id/unlock

DB-proseduuri: `sp_card_unlock(idcard)`.

**Vastaus 204 No Content** (tai 200 OK)