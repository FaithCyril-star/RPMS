const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const DeviceMetaData = sq.define("devicemetadata", {
    device_id:{
      type:DataTypes.STRING,
      allowNull:false,
      primaryKey: true,
    },
    patient_id:{
      type:DataTypes.INTEGER,
      allowNull:false,
      references: {
        model: 'userdata', 
        key: 'userid'      
      }
    }
  },{
    timestamps: false
  }
);

  DeviceMetaData.sync({ alter: true }).then(() => {
    console.log("DeviceMetaData Model synced");
  });

  module.exports = DeviceMetaData;