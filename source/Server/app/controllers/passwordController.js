const Password = require('../models/passwordModel');
const axios = require('axios');
const bcrypt = require('bcrypt');

exports.checkPassword = (req, res) => {
  const enteredPassword = req.body.password;
  if (!enteredPassword) {
    return res.status(400).send("Password is required");
  }

  Password.find(enteredPassword, (err, result) => {
    if (err) {
      return res.status(500).send("Database error: " + err);
    }

    if (result.length > 0) {
      // Password exists, unlock the door
      sendUnlockCommand(res);
    } else {
      // Password does not exist, hash it and insert it into the database
      bcrypt.hash(enteredPassword, 10, (err, hash) => {
        if (err) {
          return res.status(500).send("Error hashing password: " + err);
        }
        Password.save(hash, (err, result) => {
          if (err) {
            return res.status(500).send("Database error: " + err);
          }
          sendUnlockCommand(res);
        });
      });
    }
  });
};

function sendUnlockCommand(res) {
  axios.post('http://' + process.env.ESP32_IP_ADDRESS + '/unlock')
    .then((response) => {
      res.send("Password is correct, door unlocked");
    })
    .catch((err) => {
      console.error(err);
      res.status(500).send("Error when connecting to ESP32");
    });
}

// New method to get all passwords (for demonstration purposes only, normally not advisable for production)
exports.getPasswords = (req, res) => {
  Password.getAll((err, results) => {
    if (err) {
      return res.status(500).send("Database error: " + err);
    }
    res.json(results);
  });
};
