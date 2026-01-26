// this file is express router for card-account related routes

// Express and router
const express = require('express');
const router = express.Router();
const { authenticateToken } = require('../middleware/auth');
const { requireAdmin } = require('../middleware/authorize');

// Controller function imports
const { getCardAccountById, createCardAccount, updateCardAccount, deleteCardAccount} = require('../controllers/cardaccountcontroller');

// AMIN routes for managing card-account links
router.get('/:idCard', authenticateToken, requireAdmin, getCardAccountById);         // READ - Reads the account(s) that cards are linked to
router.post('/', authenticateToken, requireAdmin, createCardAccount);                // CREATE - Links a card to an account
router.put('/:idCard', authenticateToken, requireAdmin, updateCardAccount);          // UPDATE - Updates card to different account
router.delete('/:idCard', authenticateToken, requireAdmin, deleteCardAccount);       // DELETE - Deletes the link between card and account

// Router export
module.exports = router;