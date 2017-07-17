import * as mongoose from 'mongoose';
import { Document, Model, Schema } from 'mongoose';

import { User, UserModel } from './user';

export type Language = 'javascript' | 'python' | 'ruby';

interface SubmissionStatic {
  findByUser(user: UserModel): Promise<SubmissionModel[]>;
}

class SubmissionStatic {
  static async findByUser(user: UserModel) {
    return await Submission.find({ user_id: user._id });
  }
}

export class SubmissionClass {
  user_id: string;
  language: Language;
  code: string;
  createdAt?: Date;
  async User(): Promise<UserModel> {
    return await User.findOne({_id: this.user_id});
  }
}

export type SubmissionModel = SubmissionClass & Document;
type SubmissionType = SubmissionClass & SubmissionStatic & Model<SubmissionModel>;

const schema = new Schema({
  user_id: String,
  language: String,
  code: String,
}, {
  timestamps: { createdAt: 'created_at' }
});

schema.set('toJSON', {
  id: true,
  getters: true,
  versionKey: false,
  transform: (_: any, ret: Document) => {
    delete ret._id;
    return ret;
  }
});

schema.loadClass(SubmissionClass);
schema.loadClass(SubmissionStatic);

export const Submission = mongoose.model<SubmissionModel>('Submission', schema) as SubmissionType;

export default Submission;
