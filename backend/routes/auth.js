const express = require('express');
const router = express.Router();
const { login, logout } = require('../controllers/authController');

// POST /auth/login - Card + PIN authentication
router.post('/login', login);

// POST /auth/logout - Client-side token removal
router.post('/logout', logout);

module.exports = router;
