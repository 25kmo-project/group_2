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

Suorita seed virtuaalikoneella:
mysql -u käyttäjä -p bank_db < seed.sql

## widget

Testitunnukset:
user
pass
