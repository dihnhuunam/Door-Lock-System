const express = require("express");
const router = express.Router();
const passwordController = require("../controllers/passwordController");

// Post request to check password
router.post('/check_password', passwordController.checkPassword)

module.exports = router;