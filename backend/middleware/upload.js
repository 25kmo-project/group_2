const multer = require("multer");

const upload = multer({
  storage: multer.memoryStorage(), // NEVER write raw file to disk
  limits: {
    fileSize: 5 * 1024 * 1024 // 5MB
  },
  fileFilter: (req, file, cb) => {
    if (file.mimetype !== "image/png") {
      return cb(new Error("Only PNG images allowed"), false);
    }
    cb(null, true);
  }
});

module.exports = upload;
