const DeviceData = require("../models/DeviceData");
const { Op } = require('sequelize');


async function getDeviceData(req,res){
    const deviceId = req.query.deviceid;
    const startDate = new Date(req.query.startdate);
    const endDate = new Date(req.query.enddate);
    endDate.setHours(23, 59, 59, 999);

    try{
    const deviceData = await DeviceData.findAll({where: {device_id:deviceId,timestamp:{[Op.gte]:startDate,[Op.lte]:endDate}}});
    return res.status(200).json({data:deviceData});
    }
    catch(err){
        return res.status(500).send(err.message);
    }
}


module.exports = getDeviceData;