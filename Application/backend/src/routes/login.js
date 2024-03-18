const express = require("express");
const router = express.Router();
const { isEmail}= require('validator');
const login = require("../controllers/login");

router.post("/", async function (req,res) {
    const { email, password } = req.body;

    if (!email || !password) {
        return res.status(400).send("Please enter a valid password or email");
    }
    
    // Validate the email address
    if (!isEmail(email)) {
        return res.status(400).send("Please enter a valid email address");
    }

    return login(req,res);
})


module.exports = router;