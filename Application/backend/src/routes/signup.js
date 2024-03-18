const express = require("express");
const router = express.Router();
const { isNumeric, isEmail, isLength, isMobilePhone } = require('validator');
const isNumber = require("../utils/isNumber");
const validateDeviceId = require("../utils/validateDeviceId");
const signup = require("../controllers/signup");


//base route to use this endpoint(create)
router.post("/register-user", async function (req, res) {
const { username, password, email, phoneNumber, deviceId } = req.body;

// Validate the form data
if (!username || !password || !email || !phoneNumber || !deviceId) {
    return res.status(400).send("Please enter a valid username, password, email, phoneNumber or deviceId");
}

// Validate the email address
if (!isEmail(email)) {
    return res.status(400).send("Please enter a valid email address");
}

//validate the phone number
if (!isMobilePhone(phoneNumber,'any',{
    strictMode:true
})) {
    return res.status(400).send("Please enter a valid phone number");
}

//validate deviceId
if(!await validateDeviceId(deviceId)){
    return res.status(404).send("Device not found");
}
  
return signup.addUser(req, res);
});


router.post("/verify-code", function (req, res) {   
    const { userId, verificationCode} = req.body;
    
    //validate email
    if(!userId || !isNumber(userId)){
        return res.status(400).send("Please enter a valid user id")
    }

    //validate code
    if(!verificationCode || !(isNumeric(verificationCode) && isLength(verificationCode, { min: 6, max: 6 }))){
        return res.status(400).send("Please enter a valid verificationCode")
    }

    return signup.verifyVerificationCode(req, res);
    });


router.post("/authorise-user", function (req, res) {    
    const { userId,patientId } = req.body;

    // Validate the email address
    if (!userId || !patientId || !isNumber(userId) || !isNumber(patientId) ) {
        return res.status(400).send("Please enter valid user ids");
    }

    return signup.authoriseUser(req, res);
    });


module.exports = router;