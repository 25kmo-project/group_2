// Authentication controller for card login
const AppError = require('../middleware/AppError');
const pool = require('../db');
const bcrypt = require('bcrypt');
const jwt = require('jsonwebtoken');
const config = require('../config');

// Map procedure errors
function mapProcedureSignalToHttp(err) {
    const msg = (err?.sqlMessage || '').toLowerCase();
    if (msg.includes('card not found')) return new AppError('Card not found', 404);
    if (msg.includes('card is locked')) return new AppError('Card is locked', 403);
    return null;
}

// LOGIN with card + PIN
async function login(req, res, next) {
    try {
        const { idCard, pin } = req.body;

        if (!idCard || !pin) {
            throw new AppError('idCard and pin are required', 400);
        }

        // Get card info 
        const [cardResults] = await pool.execute('CALL sp_read_card_info(?)', [idCard]);
        const cardRows = cardResults[0] || [];

        if (!cardRows.length) {
            throw new AppError('Invalid credentials', 401);
        }

        const card = cardRows[0];

        // Check if card is locked
        if (card.is_locked) {
            throw new AppError('Card is locked', 403);
        }

        // Verify PIN with pepper (cardPIN already retrieved from sp_read_card_info)
        const pepperedPIN = pin + config.pinPepper;
        const isMatch = await bcrypt.compare(pepperedPIN, card.cardPIN); // Korjattu virhe

        if (!isMatch) {
            throw new AppError('Invalid credentials', 401);
        }

        // Get user info including role
        const [userResults] = await pool.execute('CALL sp_read_user_info(?)', [card.iduser]);
        const userRows = userResults[0] || [];
        const userRole = userRows.length ? userRows[0].role : 'user';

        // Get linked accounts using cardaccount procedure
        const [linkResults] = await pool.execute('CALL sp_get_card_info(?)', [idCard]);
        const links = linkResults[0] || [];
        
        // Get full account details for each linked account
        const accounts = [];
        for (const link of links) {
            const [accResults] = await pool.execute('CALL sp_read_account_info(?)', [link.idaccount]);
            const accRows = accResults[0] || [];
            if (accRows.length) {
                accounts.push(accRows[0]);
            }
        }

        // Generate JWT token with role
        const token = jwt.sign(
            { 
                idCard: card.idcard, 
                idUser: card.iduser,
                role: userRole
            },
            config.jwtSecret,
            { expiresIn: '10min' }
        );

        res.status(200).json({
            token,
            card: {
                idCard: card.idcard,
                idUser: card.iduser,
                isLocked: Boolean(card.is_locked)
            },
            accounts: accounts.map(acc => ({
                idAccount: acc.idaccount,
                type: acc.creditlimit > 0 ? 'credit' : 'debit',
                balance: parseFloat(acc.balance),
                creditLimit: parseFloat(acc.creditlimit)
            })),
            requiresAccountSelection: accounts.length > 1
        });
    } catch (err) {
        const appError = mapProcedureSignalToHttp(err);
        if (appError) return next(appError);
        next(err);
    }
}

// LOGOUT 
async function logout(req, res, next) {
    try {
        // Get token from Authorization header
        const authHeader = req.headers['authorization'];
        const token = authHeader && authHeader.split(' ')[1];
        
        if (token) {
            // Blacklist the token
            const { invalidateToken } = require('../middleware/auth');
            invalidateToken(token);
        }
        
        res.status(204).send();
    } catch (err) {
        next(err);
    }
}

module.exports = { login, logout };
