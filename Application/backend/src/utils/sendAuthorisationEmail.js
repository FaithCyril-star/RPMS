const nodemailer = require("nodemailer");


const baseUrl = process.env.BASE_URL || 'http://localhost:3000'; 
const sendEmail = async (requesterInfo, patientInfo, deviceId) => {
  try {
    const transporter = nodemailer.createTransport({
      service: process.env.EMAIL_SERVICE,
      port:465,
      secure:true,
      logger:true,
      debug:true,
      secureConnection:false,
      auth: {
        user: process.env.EMAIL_USERNAME,
        pass: process.env.EMAIL_PASSWORD,
      },
      tls:{
        rejectUnauthorized: true
      }
    });

    await transporter.sendMail({
      from: `"Remote Patient Monitoring System" <${process.env.EMAIL_USERNAME}>`,
      to: patientInfo.email,
      subject:"Authorization Request for Remote Patient Monitoring System",
      html: `<body style="font-family: Arial, sans-serif; font-size: 14px; line-height: 1.5; padding: 20px; text-align: center;">
      <h1>Authorization Request</h1>
      <p>Hello,</p>
      <p>A request has been made to access your device in the Remote Patient Monitoring System.</p>
      <p>The requester's information:</p>
      <ul style="list-style: none; padding: 0;">
        <li>Name: ${requesterInfo.username}</li>
        <li>Email: ${requesterInfo.email}</li>
        <li>Phone number: ${requesterInfo.phone_number}</li>
      </ul>
      <p>If you authorize this access, please click the button below:</p>
      <a href=${baseUrl}/signup/authorise-user?userid=${requesterInfo.userid}&deviceid=${deviceId} style="display: inline-block; background-color: #007bff; color: #ffffff; text-decoration: none; padding: 10px 20px; border-radius: 5px; margin-top: 20px;">Authorize Access</a>
      <p>If you did not initiate this request or do not wish to authorize access, please ignore this email.</p>
      <p>Thanks,</p>
      <p>RPMS</p>
    </body>`
    });
    console.log("Email sent sucessfully");
  } catch (error) {
    console.log("Email not sent");
    console.log(error);
  }
};

module.exports = sendEmail;