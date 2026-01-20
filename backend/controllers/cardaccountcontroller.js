// This file is cardAccountController, fully procedural version

// Imports
const AppError = require('../middleware/AppError');
const pool = require('../db');

// Controller functions for card-account related operations would go here
function mapuserRow(row) {
    return {
        idCard: row.idcard,
        idAccount: row.idaccount
    };
}

// This is error mapper
function mapProcedureSignalToHttp(err) {
    const raw = err?.sqlMessage || err?.message || '';
    const msg = raw.toLowerCase();

    if (msg.includes('card not found')) return new AppError('Card not found', 404);
    if (msg.includes('account not found')) return new AppError('Account not found', 404);
    
    if (msg.includes('card account link already exists')) return new AppError('Card account link already exists', 409);
    if (msg.includes('cannot link a locked card')) return new AppError('Cannot link a locked card', 409);
    if (msg.includes('link between card and account not found')) return new AppError('Link between card and account not found', 404);
    if (msg.includes('new account not found')) return new AppError('New account not found', 404);
    if (msg.includes('cannot change card between credits and debits')) return new AppError('Cannot change card between credits and debits', 409);
    
    if (msg.includes('cannot be null') ||
        msg.includes('cannot be null or empty') ||
        msg.includes('must be greater than') ||
        msg.includes('invalid')) {
        return new AppError(raw, 400);
    }
    if (err?.code === 'ER_SIGNAL_EXCEPTION') return new AppError(raw || 'Bad Request', 400);
    
    return null;
}

// getCardAccountById
async function getCardAccountById(req, res, next) {
    try {
        const idCard = req.params?.idCard;
        if (!idCard) throw new AppError('Card id is required', 400);

        // Call stored procedure
        const [results] = await pool.execute('CALL sp_get_card_info(?)', [idCard]);
        const rows = results[0] || [];

        if (!rows.length) throw new AppError('Card account not found', 404);

        const cardAccount = rows.map(mapuserRow);
        res.status(200).json({ data: cardAccount });
    } catch (err) {
        const appError = mapProcedureSignalToHttp(err);
        if (appError) return next(appError);
        next(err);
    }
}

// createCardAccount
async function createCardAccount(req, res, next) {
    try {
        const { idCard, idAccount } = req.body;
        if (!idCard || !idAccount) throw new AppError('Card id and account id are required', 400);
        // Call stored procedure
        await pool.execute('CALL sp_card_to_account(?, ?)', [idCard, idAccount]);
        res.status(201).json({ idCard, idAccount });
    } catch (err) {
        const appError = mapProcedureSignalToHttp(err);
        if (appError) return next(appError);
        next(err);
    }
}

// updateCardAccount
async function updateCardAccount(req, res, next) {
    try {
        const idCard = req.params?.idCard;
        const { idAccount, newIdAccount } = req.body;
        if (!idCard || !idAccount || !newIdAccount) throw new AppError('Card id, account id and new account id are required', 400);

        // Call stored procedure
        await pool.execute('CALL sp_update_card_linked_account(?, ?, ?)', [idCard, idAccount, newIdAccount]);
        res.status(200).json({ idCard, idAccount, newIdAccount });
    } catch (err) {
        const appError = mapProcedureSignalToHttp(err);
        if (appError) return next(appError);
        next(err);
    }
}

// deleteCardAccount
async function deleteCardAccount(req, res, next) {
    try {
        const idCard = req.params?.idCard;
        const { idAccount } = req.body;
        
        if (!idCard || !idAccount) throw new AppError('Card id and account id are required', 400);
        // Call stored procedure
        await pool.execute('CALL sp_remove_card_from_account(?, ?)', [idCard, idAccount]);
        res.status(204).end();
    } catch (err) {
        const appError = mapProcedureSignalToHttp(err);
        if (appError) return next(appError);
        next(err);
    }
}   




// Export controller functions
module.exports = {getCardAccountById, createCardAccount, updateCardAccount, deleteCardAccount};
