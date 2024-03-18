const express = require("express");
const router = express.Router();
const {isDate} = require('validator');
const getDeviceData = require("../controllers/getDeviceData");
const auth = require('../middlewares/auth');
const validateDeviceId = require("../utils/validateDeviceId");


router.get("/", auth.authenticate('jwt', { session: false }), async function (req,res){
    const deviceId = req.query.deviceid;
    const startDate = req.query.startdate;
    const endDate = req.query.enddate;

    if(!deviceId || !startDate || !endDate){
        return res.status(400).send("DeviceId, start date or end date is not valid");
    }

    //validate deviceId
    if(!await validateDeviceId(deviceId)){
        return res.status(404).send("Device not found");
    }

    // Validate the end and start date format
    if (!(isDate(startDate) && isDate(endDate))) {
        return res.status(400).send("Please enter a valid start date or end date");
    }

    return getDeviceData(req,res);
})


module.exports = router;