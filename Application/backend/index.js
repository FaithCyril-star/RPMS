const express = require('express');
require("./src/configs/postgres").connectDatabase();

const app = express();
app.use(express.json());
app.use(express.urlencoded());

//routers
const signupRouter = require("./src/routes/signup");
const loginRouter = require("./src/routes/login");
const getDeviceIdsRouter = require("./src/routes/getDeviceIds");
const getDeviceDataRouter = require("./src/routes/getDeviceData");
const addDeviceRouter = require("./src/routes/addDevice");
const removeDeviceRouter = require("./src/routes/removeDevice");

//endpoints
app.use("/signup",signupRouter);
app.use("/login",loginRouter);
app.use("/device-ids",getDeviceIdsRouter);
app.use("/device-data",getDeviceDataRouter);
app.use("/add-device",addDeviceRouter);
app.use("/remove-device",removeDeviceRouter);

app.get('/', (req, res) => {
  res.send("Backend is working!");
});

//start server
const port = process.env.BACKEND_SERVER_PORT;
app.listen(port, () => {
  console.log(`Example app listening at http://localhost:${port}`);
});
