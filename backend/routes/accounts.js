// Tämä tiedosto on Express-reititin accounts-resurssille

// Express ja router
const express = require('express');
const router = express.Router();

// Controller-funktioiden tuonti
const {
    getAccountById,
    createAccount,
    updateAccountCreditLimit,
    deleteAccount,
} = require('../controllers/accountsController');

// Reittien määrittely
router.get('/:id', getAccountById); // READ - hakee tilin
router.post('/', createAccount); // CREATE - tekee tilin
router.put('/:id', updateAccountCreditLimit); // UPDATE - päivittää tilin creditlimitin tiedot
router.delete('/:id', deleteAccount); // DELETE - poistaa tilin

// Routerin export
module.exports = router;