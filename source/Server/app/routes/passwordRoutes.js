const express = require("express");
const router = express.Router();
const passwordController = require("../controllers/passwordController");

router.post("/unlock", passwordController.unlock);
router.post("/changePassword", passwordController.changePassword);

module.exports = router;
