const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const UserDevice = sq.define("userdevice", {
    user_email:{
      type:DataTypes.STRING,
      allowNull:false,
      primaryKey: true
    },
    device_id:{
      type:DataTypes.STRING,
      allowNull:false,
      primaryKey: true
    },
    is_authorised:{
      type:DataTypes.BOOLEAN,
      allowNull:false,
      defaultValue:false
    }
  },{
    timestamps: false
  }
);

  UserDevice.sync({ alter: true }).then(() => {
    console.log("UserDevice Model synced");
  });

  module.exports = UserDevice;