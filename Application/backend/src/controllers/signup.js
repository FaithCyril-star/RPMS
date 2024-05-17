const bcrypt = require('bcrypt');
const randomstring = require('randomstring');
const saltRounds = 10;
const UserData = require("../models/UserData");
const DeviceMetaData = require("../models/DeviceMetaData");
const UserDevice = require("../models/UserDevice");
const sendVerificationEmail = require("../utils/sendVerificationEmail");
const sendAuthorisationEmail = require("../utils/sendAuthorisationEmail");

async function addUser(req, res) {
    const { username, password, email, phoneNumber, deviceId } = req.body;

    try {
        let existingUser = await UserData.findOne({ where: { email: email } });
        if (existingUser) {
            if (existingUser.is_verified) {
                return res.status(400).send("Email already exists");
            } else {
                return res.status(400).send("Unverified email. Please check inbox to verify");
            }
        }

        existingUser = await UserData.findOne({ where: { username: username } });
        if (existingUser) {
            return res.status(400).send("Username already exists");
        }

        existingUser = await UserData.findOne({ where: { phone_number: phoneNumber } });
        if (existingUser) {
            return res.status(400).send("Phone number already exists");
        }

        const hashedPassword = await bcrypt.hash(password, saltRounds);
        user = {
            username: username,
            password: hashedPassword,
            email: email,
            phone_number: phoneNumber
        };

        try {

            const newUser = await UserData.create(user);
            const verificationCode = randomstring.generate({
                length: 6,
                charset: 'numeric'
                });

            sendVerificationEmail(email,verificationCode);
            await newUser.update({ verification_code: verificationCode });
            
            const device = await DeviceMetaData.findOne({ where: { device_id: deviceId } });
            if(!device){
                await DeviceMetaData.create({"device_id":deviceId,"patient_id":newUser.userid});
            }
            
            const userDevice = await UserDevice.create({"user_id":newUser.userid,"device_id":deviceId});

            return res.status(201).json({"user_id":newUser.userid,"message":"User was created successfully. Please check inbox for verification code"});
        } catch (err) {
            return res.status(500).send(err.message);
        }
    } catch (err) {
        return res.status(500).send(err.message);
    }
}


async function verifyVerificationCode(req,res){
    const { userId, verificationCode} = req.body;
    try{
    const user =  await UserData.findOne({ where: { userid: userId} });
    if(!user){
        return res.status(400).send("User does not exist");
    }

    if(user.is_verified){
        return res.status(400).send("User is already verified");
    }

    const expectedVerificationCode = user.verification_code;
    if(expectedVerificationCode == verificationCode){
        await user.update({"is_verified":true});
        const userDevice = await UserDevice.findOne({ where: { user_id: userId} });
        const metaData =  await DeviceMetaData.findOne({ where: { device_id: userDevice.device_id} });
        const patientId = metaData.patient_id;
        const patient = await UserData.findOne({where:{userid:patientId}});
        sendAuthorisationEmail(user.dataValues,patient.dataValues,userDevice.device_id);
        return res.status(200).send("Email verification successful, authorisation email for device has been sent.");
    }
    else{
        return res.status(400).send("Code is invalid");
    }
}
    catch(err){
        return res.status(500).send(err.message);
    }
}


async function authoriseUser(req,res){
    const userId = req.query.userid;
    const deviceId = req.query.deviceid;

    try{
        const user = await UserData.findOne({where:{userid:userId}});
        if(!user){
            return res.status(400).send("User does not exist")
        }

        const userDevice =  await UserDevice.findOne({ where: { user_id: userId,device_id:deviceId} });
        if(!userDevice){
            return res.status(400).send("User has not requested access to this patient's device");
        }
        await userDevice.update({ is_authorised: true });
        await userDevice.save();
        return res.status(200).send(`User is successfully authorised to access ${userDevice.device_id}`);
    }
    catch(err){
        return res.status(500).send(err.message);
    }
}


module.exports = { addUser,verifyVerificationCode,authoriseUser };
