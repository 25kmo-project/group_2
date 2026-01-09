// Tämä tiedosto määrittelee oman virheluokan, jota käytetään sovelluksen virheiden käsittelyssä

// Luo uuden virheluokan, perii JavaScriptin sisäänrakennetun Error-luokan ominaisuudet
class AppError extends Error {
    // Konstruktorissa määritellään virheviesti ja HTTP-statuskoodi
    constructor(message, statusCode) {
        // Kutsuu vanhemman luokan (Error) konstruktorin. Asettaa virheviestin. Mahdollistaa automaattisen stack-tracen luomisen
        super(message);
        // Tallentaa HTTP-statuskoodin virheeseen ja Error handler lukee sen sieltä
        this.statusCode = statusCode;
        // Merkitsee virheen hallittavaksi / odotetuksi. Esimerkkejä -> virheellinen syöte, ei oikeuksia ja ei löydy
        this.isOperational = true;
    }
}

// Vie luokan käyttöön muualla, esim. controllerissa
module.exports = AppError;
