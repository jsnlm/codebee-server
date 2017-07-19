import { Mockgoose } from 'mockgoose';
import * as mongoose from 'mongoose';
import * as autoIncrement from 'mongoose-auto-increment';

import logger from './debug';
import env from './env';

const debug = logger('mongodb');

export let mockgoose: Mockgoose;

export async function initialize(mock = false) {
  const uri = env.MONGO_URI;
  let options;

  try {
    if (mock) {
      mockgoose = new Mockgoose(mongoose);
      await mockgoose.prepareStorage();
      debug('Using mock database with mockgoose');
    } else {
      options = {
        user: env.MONGO_USER,
        pass: env.MONGO_PASSWORD,
      };
    }
    
    (mongoose as any).Promise = global.Promise;
    await mongoose.connect(uri, options);
  } catch (e) {
    debug(`Error connecting to MongoDB at ${uri}`);
    throw e;
  }

  debug(`Connected to MongoDB at ${uri}`);
  autoIncrement.initialize(mongoose.connection);
}
