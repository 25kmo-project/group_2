var express = require('express');
var router = express.Router();

const { getUserById, createUser, deleteUser } = require('../controllers/usersController');

router.get('/:id', getUserById);
router.post('/', createUser);
router.delete('/:id', deleteUser);

module.exports = router;
