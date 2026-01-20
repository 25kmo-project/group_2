const express = require("express");
const bcrypt = require("bcrypt");
const jwt = require("jsonwebtoken");
const e = require("express");
const JWT_SECRET = "Yupper9538"

const app = express();
const PORT = 3000;

// Middleware
app.use(express.json());

// Fake database 
const users = [];

function authenticateToken(req, res, next) {
  const authHeader = req.headers["authorizaion"];
  const token = authHeader && authHeader.split(" ")[1];// BEARER <token>

  if (!token) return res.status(401).json({message: "No token provided"}); //401
  jwt.verify(token, JWT_SECRET, (err, user) => {
  if (err) return res.status(403).json({ message: "invalid token"}); // 403
  req.user = user;
  next();
  });
}
/**
 * REGISTER USER
 * POST /register
 * body: { "username": "john", "password": "123456" }
 */
app.post("/register", async (req, res) => {
  try {
    const { username, password } = req.body;

    if (!username || !password) {
      return res.status(400).json({ message: "Username and password required" });
    }

    // Check if user already exists
    const existingUser = users.find(u => u.username === username);
    if (existingUser) {
      return res.status(409).json({ message: "User already exists" });
    }

    // Hash password
    const saltRounds = 10;
    const hashedPassword = await bcrypt.hash(password, saltRounds);

    // TEST CONFIRM 
    console.log("PLAIN PASSWORD:", password);
    console.log("HASHED PASSWORD:", hashedPassword);

    // Save user
    users.push({
      username,
      password: hashedPassword
    });

    res.status(201).json({ message: "User registered successfully" });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

/**
 * LOGIN USER
 * POST /login
 * body: { "username": "john", "password": "123456" }
 */
app.post("/login", async (req, res) => {
  try {
    const { username, password } = req.body;

    // TEST CONFIRM LOG BASE
    console.log("LOGIN ATTEMPT:");
    console.log("Username:", username);
    console.log("Entered password:", password);

    const user = users.find(u => u.username === username);
    if (!user) {
      return res.status(401).json({ message: "Invalid credentials" });
    }
    // TEST CONFIRM HASH
    console.log("Stored hash:", user.password);

    // Compare password
    const isMatch = await bcrypt.compare(password, user.password);

    //TEST CONFIRM MATCHING PASSWORD
    console.log("Password match:", isMatch);

    if (!isMatch) {
      return res.status(401).json({ message: "Invalid credentials" });
    }
  
    const token = jwt.sign({ username}, JWT_SECRET, {expiresIn: "1h"})
    console.log("Generated JWT token:", token);

    res.json({ message: "Login successful", token });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

//PROTECTED ROUTE
app.get("/users", authenticateToken, (req, res) => {
  res.json(users);

});

// Start server
app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});
