import * as casual from 'casual';

import { User, UserClass, UserModel } from '../models/user';

casual.define('UserClass', ():UserClass => { return {
  name: casual.full_name,
  username: casual.username,
  email: casual.email,
  password: casual.password,
}});

export async function seedUser(presets = {}): Promise<UserModel> {
  return await User.register(Object.assign((casual as any).UserClass, presets));
}

export async function seedUsers(count: number, presets = {}): Promise<UserModel[]> {
  const users: UserModel[] = [];
  for (let i = 0; i < count; i++) {
    users.push(await seedUser(presets));
  }
  return users;
}
