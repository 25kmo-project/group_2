
//imports
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Changes a database row to an account object
function mapAccountRow(row) {
    return {
        idAccount: row.idaccount,
        idUser: row.iduser,
        balance: Number(row.balance),
        creditLimit: Number(row.creditlimit),
    };
}

// Maops a procedure signal to an HTTP error
function mapProcedureSignalToHttp(err) {
    const raw = err?.sqlMessage || err?.message || '';
    const msg = raw.toLowerCase();
  
    if (msg.includes('account not found')) return new AppError('Account not found', 404);
    if (msg.includes('user not found')) return new AppError('User not found', 404);
    if (msg.includes('insufficient balance')) return new AppError('Insufficient balance', 400);
    if (msg.includes('insufficient available credit')) return new AppError('Amount exceeds credit limit', 400);
  
    if (
        msg.includes('cannot be null') ||
    msg.includes('must be 0 or greater') ||
    msg.includes('must be greater than') ||
    msg.includes('negative')
    ) {
        return new AppError(raw, 400);
    }
  
    if (err?.code === 'ER_SIGNAL_EXCEPTION') return new AppError(raw || 'Bad Request', 400);
  
    return null;
}

// withdraw - withdraws money from account
async function withdraw(req, res, next) {
    try {
        const idAccount = req.params.id;
        const { amount } = req.body;
    
        if (!idAccount) throw new AppError('id is required', 400);
        if (!amount || amount <= 0) throw new AppError('Valid amount is required', 400);

        await pool.execute('CALL sp_withdraw(?, ?)', [idAccount, amount]);

        // Hae päivitetty saldo
        const [resultSets] = await pool.execute('CALL sp_read_account_info(?)', [idAccount]);
        const rows = resultSets?.[0] ?? [];
    
        res.status(200).json({
            idAccount: parseInt(idAccount),
            balance: rows.length ? parseFloat(rows[0].balance) : null,
            logged: true
        });
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// credit Withdraws money from account
async function cwithdraw(req, res, next) {
    try {
        const idAccount = req.params.id;
        const { amount } = req.body;
        
        if (!idAccount) throw new AppError('id is required', 400);
        if (!amount || amount <= 0) throw new AppError('Valid amount is required', 400);

        await pool.execute('CALL sp_credit_withdraw(?, ?)', [idAccount, amount]);

        // Hae päivitetty saldo
        const [resultSets] = await pool.execute('CALL sp_read_account_info(?)', [idAccount]);
        const rows = resultSets?.[0] ?? [];
        
        res.status(200).json({
            idAccount: parseInt(idAccount),
            balance: rows.length ? parseFloat(rows[0].balance) : null,
            logged: true
        });
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    } 
    
}

// getBalance - Hakee tilin saldon
async function getBalance(req, res, next) {
    try {
        const idAccount = req.params.id;
    
        if (!idAccount) throw new AppError('id is required', 400);
    
        const [resultSets] = await pool.execute('CALL sp_read_account_info(?)', [idAccount]);
        const rows = resultSets?.[0] ?? [];
    
        if (!rows.length) throw new AppError('Account not found', 404);
    
        res.status(200).json(mapAccountRow(rows[0]));
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// getLogs - Hakee tilin tapahtumahistorian
async function getLogs(req, res, next) {
    try {
        const idAccount = req.params.id;
    
        if (!idAccount) throw new AppError('id is required', 400);
    
        const [resultSets] = await pool.execute('CALL sp_read_account_logs(?)', [idAccount]);
        const logs = resultSets?.[0] ?? [];
    
        res.status(200).json({
            items: logs.map(log => ({
                idLog: log.idlog,
                time: log.time,
                balanceChange: parseFloat(log.balancechange)
            }))
        });
    } catch (err) {
        const mapped = mapProcedureSignalToHttp(err);
        if (mapped) return next(mapped);
        next(err);
    }
}

// Export
module.exports = { withdraw, cwithdraw, getBalance, getLogs };