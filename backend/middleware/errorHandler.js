// Tämä tiedosto on backendin keskitetty virheenkäsittelijä

// Tämä on Express error-handling middleware
function errorHandler(err, req, res, next) {
    // Tässä päätetään HTTP-statuskoodi
    const status = err.statusCode || 500;
    
    // Virhevastauksen muoto
    const payload = {
        error: err.name || 'Error',
        message: err.message || 'Internal Server Error',
    };
    
    if (process.env.NODE_ENV !== 'production') {
        // Debug
        payload.stack = err.stack;
    }
    
    // Vastauksen lähetys
    res.status(status).json(payload);
}

// Vie middleware-funktion ja mahdollistaa sen käytön muissa tiedostoissa
module.exports = { errorHandler };
