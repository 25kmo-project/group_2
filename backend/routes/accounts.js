// Tämä tiedosto on Express-reititin accounts-resurssille

// Express ja router
const express = require('express');
const router = express.Router();
const { authenticateToken } = require('../middleware/auth');
const { requireAdmin } = require('../middleware/authorize');

// Controller-funktioiden tuonti
const {
    getAllAccounts,
    getAccountById,
    createAccount,
    updateAccountCreditLimit,
    deleteAccount,
} = require('../controllers/accountsController');

// Admin-reitit - vain admin voi hallita kaikkia tilejä
router.get('/', authenticateToken, requireAdmin, getAllAccounts); // READ ALL - hakee kaikki tilit
router.get('/:id', authenticateToken, requireAdmin, getAccountById); // READ - hakee tilin
router.post('/', authenticateToken, requireAdmin, createAccount); // CREATE - tekee tilin
router.put('/:id', authenticateToken, requireAdmin, updateAccountCreditLimit); // UPDATE - päivittää tilin creditlimitin tiedot
router.delete('/:id', authenticateToken, requireAdmin, deleteAccount); // DELETE - poistaa tilin

// Routerin export
module.exports = router;