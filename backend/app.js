// Tämä tiedosto kokoaa Express-sovelluksen: middlewaret, reitit ja virheenkäsittelyn

// Riippuvuuksien tuonti
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

// Reittien tuonti
var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');
var accountsRouter = require('./routes/accounts');
var cardsRouter = require('./routes/cards');
var accountsCardsRouter = require('./routes/accountsCards');

// Express-sovelluksen tuonti
var app = express();

// Middlewaret
app.use(logger('dev')); // Tulostaa jokaisen HTTP-pyynnön konsoliin
app.use(express.json()); // Parsii JSON-request bodyt ja mahdollistaa req.body:n käytön
app.use(express.urlencoded({ extended: false })); // Parsii lomakedatan. Tarpeellinen esim. HTML-formien kanssa
app.use(cookieParser()); // Lukee cookiet ja mahdollistaa req.cookies:n käytön
app.use(express.static(path.join(__dirname, 'public'))); /* Tarjoaa staattisia tiedostoja. Näitä ei käytetä API:ssa, mutta Express-generator tuo ne 
oletuksena */

// Reittien kytkentä
app.use('/', indexRouter);
app.use('/users', usersRouter);
app.use('/accounts', accountsRouter);
app.use('/cards', cardsRouter);
app.use('/accounts-cards', accountsCardsRouter);

// 404-handler
app.use((req, res, next) => {
    const err = new Error('Not Found');
    err.statusCode = 404;
    next(err);
});

// Keskitetty error handler
const { errorHandler } = require('./middleware/errorHandler');
app.use(errorHandler);

// Sovelluksen vienti
module.exports = app;
