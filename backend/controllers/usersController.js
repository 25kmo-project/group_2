// Tämä tiedosto on usersControllers, joka on Express-controlleri. Toteuttaa users CRUD-endpointtien toiminnan.
// Käyttää proseduureja, paitsi "read" ja "update" osalta. Nämä päivitetään tänne, kunhan ne on tehty procedures.sql -tiedostoon.

// Importit
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Muuntaa DB:n sarakenimet API:n JSON-muotoon
function mapUserRow(row) {
    return {
        idUser: row.iduser,
        firstName: row.fname,
        lastName: row.lname,
        streetAddress: row.streetaddress,
    };
}

// Hakee id:n URL-parametreista kahdella vaihtoehtoisella nimellä
function getIdParam(req) {
    return req.params?.idUser ?? req.params?.id;
}

// Yrittää tulkita proseduurin SIGNAL-virheet ja muuntaa ne HTTP-statuksiksi
function mapProcedureSignalToHttp(err) {
    const msg = (err?.sqlMessage || err?.message || '').toLowerCase();
    
    if (msg.includes('already exists')) return new AppError('Customer already exists', 409);
    if (msg.includes('not found')) return new AppError('Customer not found', 404);
    if (msg.includes('cannot delete')) return new AppError('Customer cannot be deleted because it has related data', 409);
    if (err?.code === 'ER_SIGNAL_EXCEPTION') return new AppError(err.sqlMessage || 'Bad Request', 400);
    
    return null;
}

// getUserById
// Ottaa id:n URL:sta, ajaa SELECT. Jos ei löydy -> 404, muuten palauttaa käyttäjän JSON:na
// Huom: sitten kun procedures.sql sisältää "read" proseduurin, tämä päivitetään käyttämään sitä
async function getUserById(req, res, next) {
    try {
        const idUser = getIdParam(req);
        if (!idUser) throw new AppError('idUser is required', 400);
        
        const [rows] = await pool.execute(
            'SELECT iduser, fname, lname, streetaddress FROM users WHERE iduser = ?',
            [idUser]
        );
        
        if (rows.length === 0) throw new AppError('Customer not found', 404);
        
        res.status(200).json(mapUserRow(rows[0]));
    } catch (err) {
        next(err);
    }
}

// createUser
// Validoi bodyn
// Käyttää proseduuria "sp_create_user"
async function createUser(req, res, next) {
    try {
        const { idUser, firstName, lastName, streetAddress } = req.body ?? {};
        if (!idUser) throw new AppError('idUser is required', 400);
        if (!firstName || !lastName || !streetAddress) {
            throw new AppError('firstName, lastName and streetAddress are required', 400);
        }
        
        // Kutsuu proseduuria
        await pool.execute('CALL sp_create_user(?, ?, ?, ?)', [
            idUser,
            firstName,
            lastName,
            streetAddress,
        ]);
        
        res.status(201).json({ idUser, firstName, lastName, streetAddress });
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        
        if (err?.code === 'ER_DUP_ENTRY') return next(new AppError('Customer already exists', 409));
        
        next(err);
    }
}

// updateUser
// Validoi id:n + bodyn
// Huom: sitten kun procedures.sql sisältää "update" proseduurin, tämä päivitetään käyttämään sitä
async function updateUser(req, res, next) {
    try {
        const idUser = getIdParam(req);
        if (!idUser) throw new AppError('idUser is required', 400);
        
        const { firstName, lastName, streetAddress } = req.body ?? {};
        if (!firstName || !lastName || !streetAddress) {
            throw new AppError('firstName, lastName and streetAddress are required', 400);
        }
        
        const [result] = await pool.execute(
            'UPDATE users SET fname = ?, lname = ?, streetaddress = ? WHERE iduser = ?',
            [firstName, lastName, streetAddress, idUser]
        );
        
        if (result.affectedRows === 0) throw new AppError('Customer not found', 404);
        
        res.status(200).json({ idUser, firstName, lastName, streetAddress });
    } catch (err) {
        next(err);
    }
}

// deleteUser
// Validoi id:n
// Käyttää proseduuria "sp_delete_user"
async function deleteUser(req, res, next) {
    try {
        const idUser = getIdParam(req);
        if (!idUser) throw new AppError('idUser is required', 400);
        
        // Kutsuu proseduuria
        await pool.execute('CALL sp_delete_user(?)', [idUser]);
        
        res.status(204).end();
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        
        if (err?.code === 'ER_ROW_IS_REFERENCED_2') {
            return next(new AppError('Customer cannot be deleted because it has accounts', 409));
        }
        
        next(err);
    }
}

// Näitä käytetään reitittimessä
module.exports = { getUserById, createUser, updateUser, deleteUser };
