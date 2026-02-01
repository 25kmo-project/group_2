// Tämä tiedosto on logController, proseduuripohjainen versio

// Importit
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Muuntaa lokirivit API-muotoon
function mapLogRow(row) {
    return {
        idLog: row.idlog,
        idAccount: row.idaccount,
        time: row.time,
        balanceChange: Number(row.balancechange),
    };
}

// Tämä on virhekääntäjä
function mapProcedureSignalToHttp(err) {
    const raw = err?.sqlMessage || err?.message || '';
    const msg = raw.toLowerCase();
  
    if (msg.includes('account not found')) return new AppError('Account not found', 404);
  
    if (
        msg.includes('cannot be null') ||
    msg.includes('invalid')
    ) {
        return new AppError(raw, 400);
    }
  
    if (err?.code === 'ER_SIGNAL_EXCEPTION') return new AppError(raw || 'Bad Request', 400);
  
    return null;
}

// getLogsByAccount
// Hakee tilin tapahtumalogit proseduurilla
async function getLogsByAccount(req, res, next) {
    try {
        const idAccount = req.params.idAccount;
    
        if (!idAccount) {
            throw new AppError('Account ID is required', 400);
        }
    
        // Proseduurin haku
        const [resultSets] = await pool.execute('CALL sp_read_account_logs(?)', [idAccount]);
        const rows = resultSets?.[0] ?? [];
    
        // Muunnetaan rivit API-muotoon
        const logs = rows.map(mapLogRow);
    
        res.status(200).json(logs);
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// Vie funktiot
module.exports = { getLogsByAccount};
