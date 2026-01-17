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

### Database

Suorita seed virtuaalikoneella:<br>
mysql -u käyttäjä -p bank_db < seed.sql
### Tietokanta proseduurit

#### Tietokannan hallinta

**Käyttäjän lisäys**
```sql
-- Lisää uuden käyttäjän
CALL sp_create_user(iduser, fname, lname, streetaddress);
-- Esimerkki: CALL sp_create_user('user123', 'matti', 'meikalainen', 'meikatie 1');
```

**Käyttäjän poisto**
```sql
-- Poistaa käyttäjän (käyttäjällä ei saa olla tilejä)
CALL sp_delete_user(iduser);
-- Esimerkki: CALL sp_delete_user('user123');
```

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

#### Korttien hallinta

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
### Database

Suorita seed virtuaalikoneella:
mysql -u käyttäjä -p bank_db < seed.sql

## widget

Testitunnukset:
user
pass
