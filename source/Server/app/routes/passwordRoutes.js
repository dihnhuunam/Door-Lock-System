const express = require('express');
const router = express.Router();
const passwordController = require('../controllers/passwordController');

router.post('/checkPassword', passwordController.checkPassword);
router.get('/getPasswords', passwordController.getPasswords);  // Corrected the path here

module.exports = router;
