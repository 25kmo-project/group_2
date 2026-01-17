/* Tämä tiedosto on Express-reititin accounts -resurssille. Se liittää HTTP-pyynnöt -> accountsControllerin funktioihin ja toteuttaa accounts-
taulun CRUD-reitit */

// Express Router
const express = require('express');
const router = express.Router();

// Controller-funktioiden tuonti
const {
    getAccountById,
    createAccount,
    updateAccount,
    deleteAccount,
} = require('../controllers/accountsController');

// Reitit = CRUD
router.get('/:idAccount', getAccountById); // READ - hae tili
router.post('/', createAccount); // CREATE - luo tili
router.put('/:idAccount', updateAccount); // UPDATE - päivitä tili
router.delete('/:idAccount', deleteAccount); // DELETE - poista tili

// Export
module.exports = router;
