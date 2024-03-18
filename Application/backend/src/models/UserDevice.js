const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const UserDevice = sq.define("userdevice", {
    user_id:{
      type:DataTypes.INTEGER,
      allowNull:false,
      primaryKey: true,
      references: {
        model: 'userdata', 
        key: 'userid'      
      }
    },
    device_id:{
      type:DataTypes.STRING,
      allowNull:false,
      primaryKey: true,
      references: {
        model: 'devicemetadata', 
        key: 'device_id'      
      }
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