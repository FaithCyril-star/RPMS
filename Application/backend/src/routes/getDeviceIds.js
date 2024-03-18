const express = require("express");
const router = express.Router();
const {isNumeric} = require("validator");
const getDeviceIds = require("../controllers/getDeviceIds");
const auth = require('../middlewares/auth');


router.get("/", auth.authenticate('jwt', { session: false }), function (req,res){
    const userId = req.query.userid;

    // Validate the email address
    if (!userId || !isNumeric(userId)) {
        return res.status(400).send("invalid id");
    }

    return getDeviceIds(req,res);
})


module.exports = router;