const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const DeviceData = sq.define("devicedata", {
    timestamp:{
      type:DataTypes.DATE,
      allowNull:false,
      primaryKey: true,
    },
    device_id:{
      type:DataTypes.STRING,
      allowNull:false,
      primaryKey: true,
    },
    temperature: {
      type: DataTypes.INTEGER,
      allowNull: false
    },
    heart_rate:{
      type:DataTypes.INTEGER,
      allowNull:false
    },
    oxygen_saturation:{
      type:DataTypes.INTEGER,
      allowNull:false
    },
    systolic_pressure:{
        type:DataTypes.INTEGER,
        allowNull:false,
      },
    diastolic_pressure:{
        type:DataTypes.INTEGER,
        allowNull:false,
      }
  },{
    timestamps: false
  }
);

  DeviceData.sync({ alter: true }).then(() => {
    console.log("DeviceData Model synced");
  });

  module.exports = DeviceData;

