import { Mockgoose } from 'mockgoose';
import * as mongoose from 'mongoose';
import * as autoIncrement from 'mongoose-auto-increment';

import logger from './debug';
import env from './env';

const debug = logger('mongodb');

export default async function initialize() {
  const uri = env.MONGODB;

  try {
    if (env.MOCKDB) {
      const mockgoose = new Mockgoose(mongoose);
      await mockgoose.prepareStorage();
      debug('Using mock database with mockgoose');
    }
    (mongoose as any).Promise = global.Promise;
    await mongoose.connect(uri);
  } catch (e) {
    debug(`Error connecting to MongoDB at ${uri}`);
    throw e;
  }

  debug(`Connected to MongoDB at ${uri}`);
  autoIncrement.initialize(mongoose.connection);
}
