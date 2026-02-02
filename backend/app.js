// Tämä tiedosto kokoaa Express-sovelluksen: middlewaret, reitit ja virheenkäsittelyn

// Riippuvuuksien tuonti
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var swaggerUi = require('swagger-ui-express');
var YAML = require('yamljs');

// Reittien tuonti
var indexRouter = require('./routes/index');
var authRouter = require('./routes/auth');
var usersRouter = require('./routes/users');
var cardsRouter = require('./routes/cards');
var cardsaccountsRouter = require('./routes/cardaccount');
var accountsRouter = require('./routes/accounts');
var logRouter = require('./routes/log');
var atmRouter = require('./routes/atm');

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
app.use('/auth', authRouter); // Autentikointi (login, logout)
app.use('/atm', atmRouter); // ATM-toiminnot (normaalit käyttäjät)
app.use('/users', usersRouter); // Admin: käyttäjien hallinta
app.use('/cards', cardsRouter); // Admin: korttien hallinta
app.use('/cardaccount', cardsaccountsRouter); // Admin: kortti-tili linkit
app.use('/accounts', accountsRouter); // Admin: tilien hallinta
app.use('/log', logRouter); // Admin: lokit

// Swagger UI
const openapiPath = path.join(__dirname, 'openapi.yaml');
const openapiDocument = YAML.load(openapiPath);
app.use('/docs', swaggerUi.serve, swaggerUi.setup(openapiDocument));
openapiDocument.servers = [
    {
        url: `http://${process.env.SERVER_HOST}:3000`,
        description: 'Current server'
    }
];
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
