// Lataa .env tiedoston, lukee sieltä ympäristömuuttujat ja asettaa ne Node.js:n globaaliin process.env-olioon
require('dotenv').config();

// Tämä tekee tiedostosta konfiguraatiomoduulin ja mahdollistaa konfiguraatioarvojen käytön muualla sovelluksessa
module.exports = {
    // Tarkistetaan, onko PORT asetettu. Jos on -> käytetään sitä, muuten -> oletus 3000
    port: process.env.PORT ? Number(process.env.PORT) : 3000,
    // Tämä on tietokannan asetukset yhdessä paikassa
    db: {
        host: process.env.DB_HOST,
        user: process.env.DB_USER,
        password: process.env.DB_PASSWORD,
        database: process.env.DB_NAME,
        port: process.env.DB_PORT ? Number(process.env.DB_PORT) : 3306,
    },
    // Tätä käytetään myöhemmin JWT-tokenien allekirjoittamiseen ja käyttäjän tunnistamiseen
    jwtSecret: process.env.JWT_SECRET || 'dev-secret',
    // PIN pepper - lisäturvallisuus kortin PIN-koodille
    pinPepper: process.env.PIN_PEPPER || '',
    // Optional CDN base URL for serving public assets (no trailing slash)
    cdnBase: process.env.CDN_BASE || process.env.PUBLIC_CDN || '',
};
