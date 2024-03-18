const UserData = require("../models/UserData");
const UserDevice = require("../models/UserDevice");

async function removeDevice(req,res){
    const userId = req.query.userid;
    const deviceId = req.query.deviceid;

    try{
        const user = await UserData.findOne({where:{userid:userId}});
        if(!user){
            return res.status(400).send("User does not exist")
        }

        const userDevice = await UserDevice.findOne({where:{user_id:userId,device_id:deviceId}});
        if(!userDevice){
            return res.status(400).send("User does not have access to device");
        }

        await UserDevice.destroy({where:{user_id:userId,device_id:deviceId}});
        return res.status(200).send("Device has been deleted from access successfully");
    }
    catch(err){
        return res.status(500).send(err.message);
    }

}

module.exports = removeDevice;