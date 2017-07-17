import { pick } from 'lodash';

import { setupTest, clearDatabase } from '../../test/helper';
import { seedUsers } from '../../test/user.seed';
import { User, UserClass, UserModel, LoginError } from '../user';

let users: UserModel[];

beforeAll(async () => {
  await setupTest();
  users = await seedUsers(3, {password: 'password123'});
});

afterAll(async () => {
  await clearDatabase();
});

test('find user by username', async () => {
  const user = users[0];
  const doc = await User.findByUsername(user.username);
  expect(user.toJSON()).toEqual(doc.toJSON());
});

test('find user by email', async () => {
  const user = users[0];
  const doc = await User.findByEmail(user.email);
  expect(user.toJSON()).toEqual(doc.toJSON());
});

test('register new user', async () => {
  const doc: UserClass = {
    name: 'New User',
    username: 'newusername',
    email: 'newuser@example.com',
    password: 'password123'
  };

  const user = (await User.register(doc)).toJSON();
  const newUser = pick.bind(null, user).apply(null, Object.keys(doc));

  expect(newUser).toEqual(doc);
});

test('return user on correct login', async () => {
  const user = await User.login(users[0].email, 'password123');
  expect(user.toJSON()).toEqual(users[0].toJSON());
});

test('throw error on incorrect login', async () => {
  const login = User.login(users[0].email, 'abc123');
  await expect(login).rejects.toBeInstanceOf(LoginError);
});
