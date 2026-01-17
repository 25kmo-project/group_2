// Tämä tiedosto on Express-reititin users-resurssille

// Express ja router
const express = require('express');
const router = express.Router();

// Controller-funktioiden tuonti
const { getUserById, createUser, updateUser, deleteUser } = require('../controllers/usersController');

// Reittien määrittely
router.get('/:idUser', getUserById); // READ - hakee käyttäjän
router.post('/', createUser); // CREATE - tekee käyttäjän
router.put('/:idUser', updateUser); // UPDATE - päivittää käyttäjän tiedot
router.delete('/:idUser', deleteUser); // DELETE - poistaa käyttäjän

// Routerin export
module.exports = router;
