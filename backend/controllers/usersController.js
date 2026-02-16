// Tämä tiedosto on usersController, täysin proseduuripohjainen versio

// Importit
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Muuntaa tietokantarivien kentät API-muotoon
function mapUserRow(row) {
    return {
        idUser: row.iduser,
        firstName: row.fname,
        lastName: row.lname,
        streetAddress: row.streetaddress,
        avatarUrl: row.avatarUrl || null,
        avatarType: row.avatarType || null,
        role: row.role || 'user',
    };
}

// Hakee käyttäjän-id:n URL:stä joustavasti
function getIdParam(req) {
    return req.params?.idUser ?? req.params?.id;
}

// Tämä on virhekääntäjä
function mapProcedureSignalToHttp(err) {
    const raw = err?.sqlMessage || err?.message || '';
    const msg = raw.toLowerCase();
    
    if (msg.includes('user not found')) return new AppError('Customer not found', 404);
    
    if (msg.includes('user already exists')) return new AppError('Customer already exists', 409);
    
    if (msg.includes('cannot delete user with existing')) {
        return new AppError('Customer cannot be deleted because it has related data', 409);
    }
    
    if (
        msg.includes('cannot be null') ||
        msg.includes('cannot be null or empty') ||
        msg.includes('must be greater than') ||
        msg.includes('invalid')
    ) {
        return new AppError(raw, 400);
    }
    
    if (err?.code === 'ER_SIGNAL_EXCEPTION') return new AppError(raw || 'Bad Request', 400);
    
    return null;
}

// getAllUsers
// Hakee kaikki käyttäjät proseduurilla
async function getAllUsers(req, res, next) {
    try {
        const [resultSets] = await pool.execute('CALL sp_read_all_users()');
        const rows = resultSets?.[0] ?? [];

        res.status(200).json(rows.map(mapUserRow));
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// getUserById
// Tämä hakee käyttäjän proseduurilla
async function getUserById(req, res, next) {
    try {
        const idUser = getIdParam(req);
        if (!idUser) throw new AppError('idUser is required', 400);
        
        // Proseduurin haku
        const [resultSets] = await pool.execute('CALL sp_read_user_info(?)', [idUser]);
        const rows = resultSets?.[0] ?? [];
        
        if (!rows.length) throw new AppError('Customer not found', 404);
        
        res.status(200).json(mapUserRow(rows[0]));
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// createUser
// Luo käyttäjän proseduurilla
async function createUser(req, res, next) {
    try {
        const { idUser, firstName, lastName, streetAddress, role } = req.body ?? {};
        if (!idUser) throw new AppError('idUser is required', 400);
        if (!firstName || !lastName || !streetAddress) {
            throw new AppError('firstName, lastName and streetAddress are required', 400);
        }
        
        // Proseduurin haku (role optional, defaults to 'user')
        await pool.execute('CALL sp_create_user(?, ?, ?, ?, ?)', [
            idUser,
            firstName,
            lastName,
            streetAddress,
            role || 'user',
        ]);
        
        res.status(201).json({ idUser, firstName, lastName, streetAddress, role: role || 'user' });
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        
        if (err?.code === 'ER_DUP_ENTRY') return next(new AppError('Customer already exists', 409));
        
        next(err);
    }
}

// updateUser
// Päivittää käyttäjän proseduurilla
async function updateUser(req, res, next) {
    try {
        const idUser = getIdParam(req);
        if (!idUser) throw new AppError('idUser is required', 400);
        
        const { firstName, lastName, streetAddress } = req.body ?? {};
        if (!firstName || !lastName || !streetAddress) {
            throw new AppError('firstName, lastName and streetAddress are required', 400);
        }
        
        // Proseduurin haku
        await pool.execute('CALL sp_update_user_info(?, ?, ?, ?)', [
            idUser,
            firstName,
            lastName,
            streetAddress,
        ]);
        
        res.status(200).json({ idUser, firstName, lastName, streetAddress });
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// deleteUser
// Poistaa käyttäjän proseduurilla
async function deleteUser(req, res, next) {
    try {
        const idUser = getIdParam(req);
        if (!idUser) throw new AppError('idUser is required', 400);
        
        // Proseduurin haku
        await pool.execute('CALL sp_delete_user(?)', [idUser]);
        
        res.status(204).end();
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        
        if (err?.code === 'ER_ROW_IS_REFERENCED_2') {
            return next(new AppError('Customer cannot be deleted because it has related data', 409));
        }
        
        next(err);
    }
}

// Export
module.exports = { getAllUsers, getUserById, createUser, updateUser, deleteUser };
