const UserDevice = require("../models/UserDevice");
const UserData = require("../models/UserData");
const DeviceMetaData = require("../models/DeviceMetaData");
const sendAuthorisationEmail = require("../utils/sendAuthorisationEmail");

async function addDevice(req,res){
    //receive the user id and device id
    const {userId, deviceId} = req.body;

    try{
        const user = await UserData.findOne({where:{userid:userId}});
        if(!user){
            return res.status(400).send("User does not exist")
        }

        //get patient_email
        const meta_data =  await DeviceMetaData.findOne({ where: { device_id: deviceId} });
        const patientId = meta_data.patient_id;
        
        const patient = await UserData.findOne({where: { userid: patientId}});
        sendAuthorisationEmail(user.dataValues,patient.dataValues,deviceId);
        await UserDevice.create({"user_id":user.userid,"device_id":deviceId});
        return res.status(200).send("Device added, pending approval from patient");
    }
    catch(err){
        return res.status(500).send(err.message);
    }

}


module.exports = addDevice;