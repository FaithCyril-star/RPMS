const jwt = require('jsonwebtoken');
const UserData = require('../models/UserData');
const bcrypt = require('bcrypt');


async function login(req,res) {
    try{
        const { email, password } = req.body;
        const user = await UserData.findOne({ where: { email: email} });
    
        if(!user){
            return res.status(401).send("Incorrect email");
        }
    
        // Compare the provided password with the hashed password from the database
        const passwordMatch = await bcrypt.compare(password, user.password);
        if(!passwordMatch){
            return res.status(401).send("Incorrect password")
        }
    
        const token = jwt.sign({ sub: email }, process.env.JWT_SECRET);
        res.status(200).json({ userId: user.userid, username: user.username, token: token });
    }
    catch(err){
        return res.status(500).send(err.message);
    }

}

module.exports = login;