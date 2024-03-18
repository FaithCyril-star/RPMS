const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const UserData = sq.define("userdata", {
    userid: {
      type: DataTypes.INTEGER,
      primaryKey: true,
      autoIncrement: true,
      allowNull: false
    },
    username:{
      type:DataTypes.STRING,
      allowNull:false
    },
    password:{
      type:DataTypes.STRING,
      allowNull:false
    },
    email: {
      type: DataTypes.STRING,
      allowNull: false,
      primaryKey: true,
    },
    phone_number:{
      type:DataTypes.STRING,
      allowNull:false
    },
    is_verified:{
      type:DataTypes.BOOLEAN,
      allowNull:false,
      defaultValue:false
    },
    verification_code:{
      type:DataTypes.STRING,
      allowNull:false,
      defaultValue:""
    }
  },{
    timestamps: false
  }
);

  UserData.sync({ alter: true }).then(() => {
    console.log("UserData Model synced");
  });

  module.exports = UserData;