// Authorization middleware - restricts user permissions
const AppError = require('./AppError');
const pool = require('../db');

// Checks that the user can only access their own accounts
async function authorizeOwnAccount(req, res, next) {
    try {
        const idAccount = req.params.id || req.params.idAccount || req.body.idAccount;
        
        if (!idAccount) {
            return next(new AppError('Account ID is required', 400));
        }

        // Fetch the account owner
        const [resultSets] = await pool.execute('CALL sp_read_account_info(?)', [idAccount]);
        const rows = resultSets?.[0] ?? [];
        
        if (!rows.length) {
            return next(new AppError('Account not found', 404));
        }

        const account = rows[0];
        
        // Check that the account belongs to the logged-in user
        if (account.iduser !== req.user.idUser) {
            return next(new AppError('Access denied: You can only access your own accounts', 403));
        }

        // Save account for further use
        req.account = account;
        next();
    } catch (err) {
        next(err);
    }
}

// Checks that the user has admin role
function requireAdmin(req, res, next) {
    if (!req.user || req.user.role !== 'admin') {
        return next(new AppError('Access denied: Admin privileges required', 403));
    }
    next();
}

module.exports = {
    authorizeOwnAccount,
    requireAdmin
};
