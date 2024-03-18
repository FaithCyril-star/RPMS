// const bcrypt = require('bcrypt');
// const { addUser } = require('../src/controllers/signup');
// const UserData = require("../models/UserData");

// jest.mock('bcrypt', () => ({
//     hash: jest.fn((password, saltRounds, callback) => {
//         callback(null, 'hashedPassword');
//     }),
// }));

// jest.mock('../models/UserData', () => ({
//     findOne: jest.fn(),
//     create: jest.fn(),
// }));


// describe('addUser controller', () => {
//     let req, res;

//     beforeEach(() => {
//         req = {
//             body: {
//                 username: 'testUser',
//                 password: 'testPassword',
//                 email: 'test@example.com',
//                 phone_number: '1234567890',
//             },
//         };
//         res = {
//             status: jest.fn(() => res),
//             send: jest.fn(),
//         };
//     });

//     afterEach(() => {
//         jest.clearAllMocks();
//     });

//     it('should create a new user successfully', async () => {
//         UserData.findOne
//             .mockResolvedValueOnce(null) // No existing user with email
//             .mockResolvedValueOnce(null) // No existing user with username
//             .mockResolvedValueOnce(null); // No existing user with phone number

//         await addUser(req, res);

//         expect(UserData.findOne).toHaveBeenCalledTimes(3);
//         expect(bcrypt.hash).toHaveBeenCalledWith('testPassword', 10, expect.any(Function));
//         expect(UserData.create).toHaveBeenCalledWith({
//             username: 'testUser',
//             password: 'hashedPassword',
//             email: 'test@example.com',
//             phone_number: '1234567890',
//         });
//         expect(res.status).toHaveBeenCalledWith(200);
//         expect(res.send).toHaveBeenCalledWith("User was created successfully.");
//     });

//     it('should return "Email already exists"', async () => {
//         UserData.findOne.mockResolvedValueOnce({ verified: true }); // Existing user with email

//         await addUser(req, res);

//         expect(res.status).toHaveBeenCalledWith(400);
//         expect(res.send).toHaveBeenCalledWith("Email already exists");
//     });

//     it('should return "Unverified email. Please check inbox to verify"', async () => {
//         UserData.findOne.mockResolvedValueOnce({ verified: false }); // Existing user with email

//         await addUser(req, res);

//         expect(res.status).toHaveBeenCalledWith(400);
//         expect(res.send).toHaveBeenCalledWith("Unverified email. Please check inbox to verify");
//     });

//     it('should return "Username already exists"', async () => {
//         UserData.findOne
//             .mockResolvedValueOnce(null) // No existing user with email
//             .mockResolvedValueOnce({}); // Existing user with username

//         await addUser(req, res);

//         expect(res.status).toHaveBeenCalledWith(400);
//         expect(res.send).toHaveBeenCalledWith("Username already exists");
//     });

//     it('should return "Phone number already exists"', async () => {
//         UserData.findOne
//             .mockResolvedValueOnce(null) // No existing user with email
//             .mockResolvedValueOnce(null) // No existing user with username
//             .mockResolvedValueOnce({}); // Existing user with phone number

//         await addUser(req, res);

//         expect(res.status).toHaveBeenCalledWith(400);
//         expect(res.send).toHaveBeenCalledWith("Phone number already exists");
//     });

//     it('should handle bcrypt hash error', async () => {
//         bcrypt.hash.mockImplementationOnce((password, saltRounds, callback) => {
//             callback(new Error('Hash error'));
//         });

//         await addUser(req, res);

//         expect(res.status).toHaveBeenCalledWith(500);
//         expect(res.send).toHaveBeenCalledWith("Hash error");
//     });

//     it('should handle UserData.create error', async () => {
//         UserData.findOne
//             .mockResolvedValueOnce(null) // No existing user with email
//             .mockResolvedValueOnce(null) // No existing user with username
//             .mockResolvedValueOnce(null); // No existing user with phone number
//         UserData.create.mockRejectedValueOnce(new Error('Create error'));

//         await addUser(req, res);

//         expect(res.status).toHaveBeenCalledWith(500);
//         expect(res.send).toHaveBeenCalledWith("Create error");
//     });
// });
