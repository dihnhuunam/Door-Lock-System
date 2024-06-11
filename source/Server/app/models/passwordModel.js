const db = require("../config/db");

const Password = {
  save: (password, callback) => {
    const sql = "INSERT INTO passwords (password) VALUES (?)";
    db.query(sql, [password], callback);
  },

  find: (password, callback) => {
    const sql = "SELECT * FROM passwords WHERE password = ?";
    db.query(sql, [password], callback);
  },

  getAll: (callback) => {
    const sql = "SELECT * FROM passwords";
    db.query(sql, callback);
  }
};

module.exports = Password;
