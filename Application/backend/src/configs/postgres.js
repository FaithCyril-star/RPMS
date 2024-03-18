const { Sequelize } = require("sequelize");

DATABASE_URI = `postgresql://${process.env.USERNAME}:${process.env.PASSWORD}@${process.env.HOST}:${process.env.PORT}/${process.env.DATABASE}`

//database wide options
var opts = {
  define: {
      //prevent sequelize from pluralizing table names
      freezeTableName: true
  }
}

const sequelize = new Sequelize(DATABASE_URI,opts)

const connectDatabase = async () => {
  try {
    await sequelize.authenticate();
    console.log("Connection has been established successfully.");
  } catch (error) {
    console.error("Unable to connect to the database:", error);
  }
};


module.exports = { sq: sequelize, connectDatabase };