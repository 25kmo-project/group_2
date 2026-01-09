/* Tämä tiedosto on esimerkkicontroller, jonka tarkoitus on todistaa, että HTTP-status-linjanveto, AppError ja errorHandler toimivat oikein, vaikka ei 
ole vielä tietokantaa. */

// AppErrorin tuonti
const AppError = require('../middleware/AppError');

// Hae käyttäjä ID:n perusteella. Palauta 200 OK, jos löytyy. 404 Not Found, jos ei löydy. 400 Bad Request, jos ID puuttuu.
function getUserById(req, res, next) {
    try {
        // Lukee URL-parametrin
        const id = req.params.id;
        
        // Validointi, jos ID puuttuu -> 400 Bad Request
        if (!id) {
            throw new AppError('id is required', 400);
        }
        
        // Simuloi tietokantahakua
        const user = null;
        
        // Ei löytynyt -> 404 Not Found
        if (!user) {
            throw new AppError('User not found', 404);
        }
        
        // Löytyi -> 200 OK
        res.status(200).json(user);
    } catch (err) {
        next(err);
    }
}

// Uuden käyttäjän luonti. Palauta -> 201 Created, jos onnistuu. 400 Bad Request, jos data puuttuu. 409 Conflict, jos duplikaatti
function createUser(req, res, next) {
    try {
        // Lukee JSON-bodyn
        const { name, email } = req.body;
        
        // Validointi, puuttuva kenttä -> 400
        if (!name || !email) {
            throw new AppError('name and email are required', 400);
        }
        
        // Simuloi duplikaattitarkistusta, kun vaihtaa true -> 409 Conflict
        const emailExists = false;
        
        if (emailExists) {
            throw new AppError('Email already exists', 409);
        }
        
        // Simuloi tietokantaan luotua käyttäjää
        const created = { id: 123, name, email };
        
        // Onnistunut luonti -> 201 Created
        res.status(201).json(created);
    } catch (err) {
        next(err);
    }
}

// Käyttäjän poisto. Palauta 204 No Content, jos poistettu. 404 Not Found, jos ei löydy. 400 Bad Request, jos ID puuttuu
function deleteUser(req, res, next) {
    try {
        // Lukee URL-parametrin
        const id = req.params.id;
        
        // Validointi -> 400
        if (!id) {
            throw new AppError('id is required', 400);
        }
        
        // Simuloi poiston onnistumista, jos false -> 404
        const deleted = true;
        
        if (!deleted) {
            throw new AppError('User not found', 404);
        }
        
        // Onnistunut poisto -> 204 No Content. Ei response-bodyä
        res.status(204).end();
        // Kaikki virheet ohjataan keskitetylle error handlerille. Controller ei koskaan lähetä virhe-responsea itse
    } catch (err) {
        next(err);
    }
}

// Vie controller-funktiot ja mahdollistaa niiden käytön reitittimessä
module.exports = { getUserById, createUser, deleteUser };
