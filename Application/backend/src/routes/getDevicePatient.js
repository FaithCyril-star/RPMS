const express = require("express");
const router = express.Router();
const getDevicePatient = require("../controllers/getDevicePatient");
const auth = require('../middlewares/auth');
const validateDeviceId = require('../utils/validateDeviceId');

router.get("/", auth.authenticate('jwt', { session: false }), async function (req,res){
    const deviceId = req.query.deviceid;

    // Validate the email address
    if (!deviceId || !await validateDeviceId(deviceId)) {
        return res.status(400).send("device not found");
    }

    return getDevicePatient(req,res);
})


module.exports = router;