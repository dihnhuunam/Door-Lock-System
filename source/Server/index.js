require("dotenv").config();
const express = require("express");
const bodyParser = require("body-parser");
const passwordRoutes = require("./app/routes/passwordRoutes.js");

const app = express();
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

app.use("/api", passwordRoutes);

app.get("/", (req, res) => {
  res.send("Demo Version");
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
