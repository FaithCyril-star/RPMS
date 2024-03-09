const { sq } = require("../configs/postgres");
const { DataTypes } = require("sequelize");

const UserData = sq.define("userdata", {
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
    }
  },{
    timestamps: false
  }
);

  UserData.sync({ alter: true }).then(() => {
    console.log("UserData Model synced");
  });

  module.exports = UserData;