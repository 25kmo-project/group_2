// this file is express router for card-account related routes

// Express and router
const express = require('express');
const router = express.Router();

// Controller function imports
const { getCardAccountById, createCardAccount, updateCardAccount, deleteCardAccount} = require('../controllers/cardaccountcontroller');

// Route definitions
router.get('/:idCard', getCardAccountById); // READ - Reads the account(s) that cards are linked to
router.post('/', createCardAccount); // CREATE - Links a card to an account
router.put('/:idCard', updateCardAccount); // UPDATE - Updates card to different account
router.delete('/:idCard', deleteCardAccount); // DELETE - Deletes the link between card and account

// Router export
module.exports = router;