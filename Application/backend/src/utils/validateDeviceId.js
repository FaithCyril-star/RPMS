const AWS = require('aws-sdk');

// Configure AWS SDK with your credentials and region
AWS.config.update({
  accessKeyId: process.env.ACCESS_KEY,
  secretAccessKey: process.env.SECRET_KEY,
  region: process.env.AWS_REGION
});

// Create IoT object
const iot = new AWS.Iot();

async function checkIfThingExists(deviceId) {
  try {
    // Call describeThing API to check if the thing exists
    await iot.describeThing({
        thingName: deviceId
      }).promise();
    return true; // Thing exists
  } catch (error) {
    if (error.code === 'ResourceNotFoundException') {
      return false; // Thing does not exist
    }
    throw error; // Throw error for other exceptions
  }
}

module.exports = checkIfThingExists;