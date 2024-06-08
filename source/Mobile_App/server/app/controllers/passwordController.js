const db = require("../config/db.js");
const axios = require("axios");

// Get entered password
const checkPassword = (req, res) => {
  const enteredPassword = req.body.password;
  if (!enteredPassword) {
    return res.status(400).send("Password is required");
  }

  // SQL Query to check password
  const sql = "SELECT * FROM passwords WHERE password = ?";
  db.query(sql, [enteredPassword], (err, result) => {
    if (err) {
      return res.status(500).send("Database error: " + err);
    }

    if (result.length > 0) {
      axios
        .post(ESP32_IP_ADDRESS)
        .then((response) => {
          res.send("Password is correct, door unlocked");
        })
        .catch((err) => {
          console.error(err);
          res.status(500).send("Error when connecting to ESP32");
        });
    } else {
      res.send("Password is incorrect");
    }
  });
};

module.exports = {
  checkPassword,
};
