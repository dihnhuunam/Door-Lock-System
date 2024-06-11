const Password = require("../models/passwordModel");
const axios = require("axios");

function sendUnlockCommand(res) {
  axios
    .post(`http://${process.env.ESP32_IP_ADDRESS}/unlock`)
    .then(() => {
      res.send("Password is correct, door unlocked");
    })
    .catch((err) => {
      console.error("Error when connecting to ESP32:", err);
      res.status(500).send("Error when connecting to ESP32");
    });
}

exports.unlock = (req, res) => {
  const enteredPassword = req.body.password;
  if (!enteredPassword) {
    return res.status(400).send("Password is required");
  }

  Password.find(enteredPassword, (err, result) => {
    if (err) {
      console.error("Database error:", err);
      return res.status(500).send("Database error: " + err);
    }

    if (result.length > 0) {
      sendUnlockCommand(res);
    } else {
      return res.status(401).send("Password is incorrect");
    }
  });
};

exports.getPasswords = (req, res) => {
  Password.getAll((err, results) => {
    if (err) {
      console.error("Database error:", err);
      return res.status(500).send("Database error: " + err);
    }
    res.json(results);
  });
};
