# Group 2 Project

## Backend-osio

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

### Tietokanta proseduurit

#### Tietokannan hallinta

**Käyttäjän lisäys**
```sql
-- Lisää käyttäjän
CALL sp_create_user(iduser, fname, lname,streetaddress);
-- Esimerkki: CALL sp_create_user(123,'matti', 'meikalainen','meikatie 1');
```

#### Korttien hallinta


**Kortin lukitseminen**
```sql
-- Lukitsee kortin ID:n perusteella
CALL sp_card_lock(idcard);
-- Esimerkki: CALL sp_card_lock(1);
```

**Kortin lukituksen poisto**
```sql
-- Poistaa kortin lukituksen ID:n perusteella
CALL sp_card_unlock(idcard);
-- Esimerkki: CALL sp_card_unlock(1);
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
### Database

Suorita seed virtuaalikoneella:
mysql -u käyttäjä -p bank_db < seed.sql

## widget

Testitunnukset:
user
pass
