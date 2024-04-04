const UserData = require("../models/UserData");
const DeviceMetaData = require("../models/DeviceMetaData");


async function getDevicePatient(req,res){
    const deviceId = req.query.deviceid;
    try{
    const deviceMetaData = await DeviceMetaData.findOne({where:{device_id:deviceId}});
    if(!deviceMetaData){
        return res.status(400).send("Device does not exist")
    }
    const devicePatient = await UserData.findOne({where: {userid:deviceMetaData.patient_id}});
    return res.status(200).json({ username: devicePatient.username});
    }
    catch(err){
        return res.status(500).send(err.message);
    }
}


module.exports = getDevicePatient;