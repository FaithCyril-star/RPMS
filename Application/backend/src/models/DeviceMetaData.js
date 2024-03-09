const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const DeviceMetaData = sq.define("devicemetadata", {
    device_id:{
      type:DataTypes.STRING,
      allowNull:false
    },
    patient_email:{
      type:DataTypes.STRING,
      allowNull:false
    }
  },{
    timestamps: false
  }
);

  DeviceMetaData.sync({ alter: true }).then(() => {
    console.log("DeviceMetaData Model synced");
  });

  module.exports = DeviceMetaData;