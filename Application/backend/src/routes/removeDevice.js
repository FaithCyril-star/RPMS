const express = require('express');
const router = express.Router();
const {isNumeric} = require("validator");
const validateDeviceId = require("../utils/validateDeviceId");
const removeDevice = require("../controllers/removeDevice");
const auth = require('../middlewares/auth');

router.delete("/", auth.authenticate('jwt', { session: false }), function (req,res){
    const userId = req.query.userid;
    const deviceId = req.query.deviceid;

    if(!userId || !deviceId){
        return res.status(400).send("Please enter a valid user id or device id");
    }

    if(!isNumeric(userId)){
        return res.status(400).send('Please enter a valid user id');
    }

    if(!validateDeviceId){
        return res.status(400).send('Device not found');
    }

    return removeDevice(req,res);

})

module.exports = router;