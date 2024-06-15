const db = require("../config/db");

const Password = {
  find: (callback) => {
    const sql = "SELECT * FROM passwords LIMIT 1";
    db.query(sql, callback);
  },

  update: (newPassword, callback) => {
    const sql = "UPDATE passwords SET password = ? WHERE id = 1"; // Assuming there's only one password entry
    db.query(sql, [newPassword], callback);
  },
  get: (callback) => {
    const sql = "SELECT * FROM passwords";
    db.query(sql, callback);
  },
};

module.exports = Password;
