// JWT Authentication middleware
const jwt = require('jsonwebtoken');
const config = require('../config');
const AppError = require('./AppError');

// Token blacklist (in-memory, resets on server restart)
const tokenBlacklist = new Set();

// add token to blacklist
function invalidateToken(token) {
    tokenBlacklist.add(token);
    // Remove token automatically after 10 minutes (token TTL)
    setTimeout(() => tokenBlacklist.delete(token), 10 * 60 * 1000);
}

// Middleware to verify JWT token
function authenticateToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    const token = authHeader && authHeader.split(' ')[1]; 

    if (!token) {
        return next(new AppError('No token provided', 401));
    }

    // Check if token is in blacklist
    if (tokenBlacklist.has(token)) {
        return next(new AppError('Token has been revoked', 401));
    }

    jwt.verify(token, config.jwtSecret, (err, decoded) => {
        if (err) {
            return next(new AppError('Invalid or expired token', 403));
        }
        req.user = decoded; 
        next();
    });
}

module.exports = { authenticateToken, invalidateToken };
