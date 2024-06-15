const express = require("express");
const bodyParser = require("body-parser");
const dotenv = require("dotenv");
dotenv.config();
const passwordRoutes = require("./app/routes/passwordRoutes");

const app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

app.get("/", (req, res) => {
  res.send("WELCOME TO DOOR LOCK SYSTEM SERVER");
});

app.use("/", passwordRoutes);

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});

app.post('/connect', (req, res) => {
  console.log('ESP32 connected');
  res.send('Connection successful');
});