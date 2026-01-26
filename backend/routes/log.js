// This file is an Express router for the log resource

// Express and router
const express = require('express');
const router = express.Router();
const { authenticateToken } = require('../middleware/auth');
const { requireAdmin } = require('../middleware/authorize');

// Controller function import
const { getLogsByAccount } = require('../controllers/logController');

// Admin routes - only admin can view logs
router.get('/:idAccount', authenticateToken, requireAdmin, getLogsByAccount); // READ - hakee tilin tapahtumalogit

// Routerin export
module.exports = router;
