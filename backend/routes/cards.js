// Express router for card CRUD operations

const express = require('express');
const router = express.Router();
const { authenticateToken } = require('../middleware/auth');
const { requireAdmin } = require('../middleware/authorize');

// Controller function imports
const {createCard, getCardById, getAllCards, updateCardPIN, deleteCard, lockCard, unlockCard } = require('../controllers/cardsController');

// Admin-reitit - vain admin voi hallita kaikkia kortteja
router.post('/', authenticateToken, requireAdmin, createCard);                    // CREATE - Create new card with PIN
router.get('/', authenticateToken, requireAdmin, getAllCards);                    // READ ALL - Get all cards
router.get('/:idCard', authenticateToken, requireAdmin, getCardById);             // READ - Get card by ID
router.put('/:idCard/pin', authenticateToken, requireAdmin, updateCardPIN);       // UPDATE - Update card PIN
router.delete('/:idCard', authenticateToken, requireAdmin, deleteCard);           // DELETE - Delete card
router.post('/:idCard/lock', authenticateToken, requireAdmin, lockCard);          // LOCK - Lock card
router.post('/:idCard/unlock', authenticateToken, requireAdmin, unlockCard);      // UNLOCK - Unlock card

// Router export
module.exports = router;
