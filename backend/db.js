// Tämä tiedosto luo ja vie ulos MySQL-yhteyspoolin, jota koko backend käyttää tietokantaan

// Käytetään mysql2-kirjaston promise-versiota asynkronisten toimintojen helpottamiseksi
const mysql = require('mysql2/promise');
// Lukee tietokannan asetukset
const config = require('./config');

// Ei avata vain yhtä yhteyttä, vaan yhteyspoolin
const pool = mysql.createPool({
    // Poolin asetukset
    host: config.db.host,
    user: config.db.user,
    password: config.db.password,
    database: config.db.database,
    port: config.db.port,
    // Jos kaikki yhteydet ovat käytössä, uusi kysely odottaa ja ei kaadu virheeseen
    waitForConnections: true,
    // Enintään 10 samanaikaista MySQL-yhteyttä
    connectionLimit: 10,
});

// Viedään pool-muuttuja, jotta muut moduulit voivat käyttää sitä
module.exports = pool;
