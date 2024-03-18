const express = require("express");
const router = express.Router();
const addDevice = require("../controllers/addDevice");
const auth = require('../middlewares/auth');
const isNumber = require("../utils/isNumber");
const validateDeviceId = require("../utils/validateDeviceId");


router.post("/", auth.authenticate('jwt', { session: false }), async function (req,res){
    const {userId, deviceId} = req.body;

    if(!userId || !deviceId){
        return res.status(400).send("Invalid email or deviceId");
    }

    if (!isNumber(userId)) {
        return res.status(400).send("Please enter a valid id");
    }

    //validate deviceId
    if(!await validateDeviceId(deviceId)){
        return res.status(404).send("Device not found");
    }

    return addDevice(req,res);
})


module.exports = router;