const nodemailer = require("nodemailer");

const sendEmail = async (email, verificationCode) => {
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
      to: email,
      subject: "Email verification for Remote Patient Monitoring System",
      html: `<body style="font-family: Arial, sans-serif; font-size: 14px; line-height: 1.5; padding: 20px; text-align: center;">
      <h1>Verify your email address</h1>
      <p>Hello,</p>
      <p>Thank you for signing up. Please use the following verification code:</p>
      <p style="font-size: 20px; font-weight: bold; color: red; margin-top: 20px;">${verificationCode}</p>
      <p>If you did not sign up for our service, please ignore this email.</p>
      <p>Thanks,</p>
      <p>RPMS</p>
    </body>`
    });
    console.log("code sent sucessfully");
  } catch (error) {
    console.log("code not sent");
    console.log(error);
  }
};

module.exports = sendEmail;