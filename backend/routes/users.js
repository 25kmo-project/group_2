// Tämä tiedosto on Express-reititin users-resurssille

// Express ja router
const express = require('express');
const router = express.Router();
const { authenticateToken } = require('../middleware/auth');
const { requireAdmin } = require('../middleware/authorize');

// Controller-funktioiden tuonti
const { getUserById, createUser, updateUser, deleteUser } = require('../controllers/usersController');

// Admin-reitit - vain admin voi hallita käyttäjiä
router.get('/:idUser', authenticateToken, requireAdmin, getUserById); // READ - hakee käyttäjän
router.post('/', authenticateToken, requireAdmin, createUser); // CREATE - tekee käyttäjän
router.put('/:idUser', authenticateToken, requireAdmin, updateUser); // UPDATE - päivittää käyttäjän tiedot
router.delete('/:idUser', authenticateToken, requireAdmin, deleteUser); // DELETE - poistaa käyttäjän

// Routerin export
module.exports = router;
