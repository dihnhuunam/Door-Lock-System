const Password = require("../models/passwordModel");

let doorStatus = "closed";

const passwordController = {
  unlock: (req, res) => {
    const { password } = req.body;

    Password.find((error, results) => {
      if (error) {
        return res.status(500).json({ error: "Internal server error" });
      }

      if (results.length === 0 || results[0].password !== password) {
        return res.status(401).json({ error: "Invalid password" });
      }

      doorStatus = "open"; // Set door status to open
      res.json({ message: "Unlocked successfully" });
    });
  },

  changePassword: (req, res) => {
    const { oldPassword, newPassword } = req.body;

    Password.find((error, results) => {
      if (error) {
        return res.status(500).json({ error: "Internal server error" });
      }

      if (results.length === 0 || results[0].password !== oldPassword) {
        return res.status(401).json({ error: "Invalid old password" });
      }

      Password.update(newPassword, (error, results) => {
        if (error) {
          return res.status(500).json({ error: "Internal server error" });
        }

        res.json({ message: "Password changed successfully" });
      });
    });
  },

  getPassword: (req, res) => {
    Password.get((err, results) => {
      if (err) {
        return res.status(500).json({ error: "Internal server error" });
      }

      res.json(results);
    });
  },

  checkDoorStatus: (req, res) => {
    res.json({ status: doorStatus });
    if (doorStatus === "open") {
      doorStatus = "closed"; // Reset door status after sending the status
    }
  }
};

module.exports = passwordController;
