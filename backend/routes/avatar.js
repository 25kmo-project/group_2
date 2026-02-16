const express = require("express");
const rateLimit = require("express-rate-limit");
const sharp = require("sharp");
const { v4: uuidv4 } = require("uuid");
const fs = require("fs");
const path = require("path");
const config = require("../config");
const pool = require("../db");

const multer = require("multer");
// create local multer instance to ensure .single exists
const upload = multer({
  storage: multer.memoryStorage(),
  limits: { fileSize: 1 * 1024 * 1024 },
  fileFilter: (req, file, cb) => {
    if (file.mimetype !== "image/png") return cb(new Error("Only PNG images allowed"), false);
    cb(null, true);
  }
});

const router = express.Router();

// rate limit uploads
const uploadLimiter = rateLimit({
  windowMs: 15 * 60 * 1000,
  max: 20
});

// resolve upload folders inside project's public folder
const AVATAR_DIR = path.join(__dirname, "..", "public", "uploads", "avatars");
const PRE_DIR = path.join(__dirname, "..", "public", "uploads", "pre");
const PREAVATARS_PATH = path.join(__dirname, "..", "public", "preavatars.json");
const PREAVATARS_ALT = path.join(__dirname, "..", "public", "uploads", "preavatars.json");

if (!fs.existsSync(AVATAR_DIR)){
    fs.mkdirSync(AVATAR_DIR, { recursive: true });
}
if (!fs.existsSync(PRE_DIR)){
    fs.mkdirSync(PRE_DIR, { recursive: true });
}

// DB reads/writes now in place; removed in-memory mock
let ensureAvatarColumnsPromise = null;
function ensureAvatarColumns() {
  if (!ensureAvatarColumnsPromise) {
    ensureAvatarColumnsPromise = (async () => {
      await pool.query("ALTER TABLE users ADD COLUMN IF NOT EXISTS avatarUrl VARCHAR(512) NULL");
      await pool.query("ALTER TABLE users ADD COLUMN IF NOT EXISTS avatarType VARCHAR(20) NULL");
    })().catch((err) => {
      ensureAvatarColumnsPromise = null;
      throw err;
    });
  }
  return ensureAvatarColumnsPromise;
}

function deleteAvatarFile(avatarUrl) {
  if (!avatarUrl) return;
  const filePath = path.join(__dirname, "..", "public", avatarUrl.replace(/^\//, ""));
  fs.access(filePath, fs.constants.F_OK, (err) => {
    if (err) return;
    fs.unlink(filePath, (err) => {});
  });
}

function toCdnUrl(url) {
  if (!url) return url;
  if (/^https?:\/\//i.test(url)) return url;
  const base = config.cdnBase || "";
  if (!base) return url;
  return base.replace(/\/$/, "") + url;
}

// Get pre-selected avatars by listing files in public/uploads/pre
const getPreselectedHandler = (req, res) => {
  console.log("[avatar] GET preselected:", req.originalUrl);
  let avatarsDataPath = null;
  if (fs.existsSync(PREAVATARS_PATH)) avatarsDataPath = PREAVATARS_PATH;
  else if (fs.existsSync(PREAVATARS_ALT)) avatarsDataPath = PREAVATARS_ALT;

  if (avatarsDataPath) {
    try {
      const data = fs.readFileSync(avatarsDataPath, "utf8");
      console.log("preavatars.json raw:", data);
      const avatars = JSON.parse(data).map(a => ({ ...a, url: toCdnUrl(a.url) }));
      return res.json(avatars);
    } catch (e) {
      console.error("Failed to parse preavatars.json:", e);
      return res.status(500).json({ error: "Invalid preavatars.json" });
    }
  }
  fs.readdir(PRE_DIR, (err, files) => {
    if (err) return res.status(500).json({ error: "Cannot list preselected avatars" });
    const avatars = files.filter(f => /\.(png|jpg|jpeg)$/i.test(f)).map((f, i) => ({ id: `${i + 1}`, url: `/uploads/pre/${f}` }));
    res.json(avatars);
  });
};

/* ---------------- UPLOAD AVATAR ---------------- */
const uploadAvatarHandler = async (req, res) => {
  try {
    console.log("[avatar] POST upload:", req.originalUrl);
    await ensureAvatarColumns();
    const { userId } = req.body;
    if (!userId) return res.status(400).json({ error: "userId is required" });
    if (!req.file || !req.file.buffer) return res.status(400).json({ error: "No file uploaded" });

    // Query user from DB
    const [rows] = await pool.query('SELECT iduser FROM users WHERE iduser = ?', [userId]);
    if (!rows || !rows[0]) return res.status(404).json({ error: "User not found" });

    const filename = uuidv4() + ".png";
    const filepath = path.join(AVATAR_DIR, filename);

    await sharp(req.file.buffer)
      .resize(512, 512, { fit: "cover" })
      .png({ compressionLevel: 9, adaptiveFiltering: true, force: true })
      .toFile(filepath);

    // Save to DB
    const avatarUrl = `/uploads/avatars/${filename}`;
    await pool.query('UPDATE users SET avatarUrl = ?, avatarType = ? WHERE iduser = ?', 
      [avatarUrl, 'uploaded', userId]);

    res.json({ success: true, avatar: avatarUrl });
  } catch (err) {
    console.error("Avatar upload error:", err);
    res.status(400).json({ error: "Invalid image file" });
  }
};

// Choose a pre-selected avatar
const selectPreselectedHandler = (req, res) => {
  console.log("[avatar] POST select preselected:", req.originalUrl);
  const { userId, avatarId } = req.body;
  (async () => {
    try {
      await ensureAvatarColumns();
      if (!userId || !avatarId) return res.status(400).json({ error: "userId and avatarId are required" });
      // Query user from DB
      const [rows] = await pool.query('SELECT iduser, avatarUrl, avatarType FROM users WHERE iduser = ?', [userId]);
      if (!rows || !rows[0]) return res.status(404).json({ error: "User not found" });

      const user = rows[0];

      let selectedUrl = null;
      if (fs.existsSync(PREAVATARS_PATH)) {
        const data = fs.readFileSync(PREAVATARS_PATH, "utf8");
        const avatars = JSON.parse(data).map(a => ({ ...a, url: toCdnUrl(a.url) }));
        const avatar = avatars.find(a => `${a.id}` === `${avatarId}`);
        if (!avatar) return res.status(404).json({ error: "Avatar not found" });
        selectedUrl = avatar.url;
      } else if (fs.existsSync(PREAVATARS_ALT)) {
        const data = fs.readFileSync(PREAVATARS_ALT, "utf8");
        const avatars = JSON.parse(data).map(a => ({ ...a, url: toCdnUrl(a.url) }));
        const avatar = avatars.find(a => `${a.id}` === `${avatarId}`);
        if (!avatar) return res.status(404).json({ error: "Avatar not found" });
        selectedUrl = avatar.url;
      } else {
        const files = fs.readdirSync(PRE_DIR).filter(f => /\.(png|jpg|jpeg)$/i.test(f));
        const idx = parseInt(avatarId, 10) - 1;
        if (!files[idx]) return res.status(404).json({ error: "Avatar not found" });
        selectedUrl = `/uploads/pre/${files[idx]}`;
      }

      // Delete previous uploaded avatar if it was user-uploaded
      if (user.avatarType === "uploaded" && user.avatarUrl) {
        deleteAvatarFile(user.avatarUrl);
      }

      // Update DB
      await pool.query('UPDATE users SET avatarUrl = ?, avatarType = ? WHERE iduser = ?', 
        [selectedUrl, 'preselected', userId]);

      res.json({ success: true, avatar: selectedUrl });
    } catch (err) {
      console.error("Preselected avatar select error:", err);
      res.status(500).json({ error: "Failed to select avatar" });
    }
  })();
};

// Support both mount styles:
// - app.use("/", avatarRouter)      -> use /api/... paths
// - app.use("/api", avatarRouter)   -> use /... paths
router.get("/api/avatars/preselected", getPreselectedHandler);
router.get("/avatars/preselected", getPreselectedHandler);

router.post("/api/avatar/upload", uploadLimiter, upload.single("avatar"), uploadAvatarHandler);
router.post("/avatar/upload", uploadLimiter, upload.single("avatar"), uploadAvatarHandler);

router.post("/api/avatar/select/preselected", selectPreselectedHandler);
router.post("/avatar/select/preselected", selectPreselectedHandler);

module.exports = router;

