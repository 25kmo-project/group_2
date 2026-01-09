/* Tämä tiedosto on Express-reititin, jonka ainoa tehtävä on yhdistää URL-osoitteet controller-funktioihin. Tämä ei sisällä mm. virheenkäsittelyä -> 
ne on tarkoituksella siirretty controlleriin ja error handleriin */

var express = require('express');
// Luo alireitittimen. Tämä ei ole koko sovellus, vaan yksi "reitityspala". Tämä router liitetty app.js:ssä pääsovellukseen.
var router = express.Router();

// Controller-funktioiden tuonti
const { getUserById, createUser, deleteUser } = require('../controllers/usersController');

// Vastaa HTTP GET -pyyntöihin. Palauttaa käyttäjän ID:n perusteella. Mahdolliset vastaukset controllerissa 200 OK, 404 Not Found ja 400 Bad Request
router.get('/:id', getUserById);
// Vastaa HTTP POST -pyyntöihin. Body tulee JSON-muodossa. Mahdolliset vastaukset controllerissa 201 Created, 400 Bad Request ja 409 Conflict
router.post('/', createUser);
/* Vastaa HTTP DELETE -pyyntöihin. Poistaa käyttäjän ID:n perusteella. Kutsuun controllerin deleteUser-funktiota. Mahdolliset vastaukset 
controllerissa 204 No Content, 404 Not Found ja 400 Bad Request */
router.delete('/:id', deleteUser);

// Vie routerin ulos tiedostosta ja mahdollistaa sen käytön muissa tiedostoissa
module.exports = router;
