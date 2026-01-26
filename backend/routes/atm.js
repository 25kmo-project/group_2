// ATM-toiminnot - normaalit käyttäjät (kortinhaltijat)

const express = require('express');
const router = express.Router();
const { authenticateToken } = require('../middleware/auth');
const { authorizeOwnAccount } = require('../middleware/authorize');

// Controller-funktioiden tuonti
const {
    withdraw,
    cwithdraw,
    getBalance,
    getLogs
} = require('../controllers/atmController');

// ATM-reitit - vain omille tileille
router.get('/:id', authenticateToken, authorizeOwnAccount, getBalance); // Saldon tarkistus
router.get('/:id/logs', authenticateToken, authorizeOwnAccount, getLogs); // Tapahtumahistoria
router.post('/:id/withdraw', authenticateToken, authorizeOwnAccount, withdraw); // Debit-nosto
router.post('/:id/credit/withdraw', authenticateToken, authorizeOwnAccount, cwithdraw); // Credit-nosto

module.exports = router;