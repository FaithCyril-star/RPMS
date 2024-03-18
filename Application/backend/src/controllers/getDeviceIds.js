const UserData = require("../models/UserData");
const UserDevice = require("../models/UserDevice");


async function getDeviceIds(req,res){
    const userId = req.query.userid;
    try{
    const user = await UserData.findOne({where:{userid:userId}});
    if(!user){
        return res.status(400).send("User does not exist")
    }
    const deviceIds = await UserDevice.findAll({where: {user_id:userId,is_authorised:true},attributes: ['device_id']});
    return res.status(200).json({ids:deviceIds});
    }
    catch(err){
        return res.status(500).send(err.message);
    }
}


module.exports = getDeviceIds;