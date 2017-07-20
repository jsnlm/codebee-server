import * as mongoose from 'mongoose';
import { Document, Model, Schema } from 'mongoose';

import { User, UserModel } from './user';

export type Language = 'javascript' | 'python' | 'ruby' | 'c++';
export const LanguageMeta = {
  'javascript': {
    extension:'js',
    execCommand: (fileName:string) => `node ${fileName}`
  },
  'python': {
    extension:'py',
    execCommand: (fileName:string) => `python ${fileName}`
  },
  'ruby': {
    extension:'rb',
    execCommand: (fileName:string) => `ruby ${fileName}`
  },
  'c++': {
    extension: `cpp`,
    execCommand: (fileName:string) => `./${fileName}`
  }
}

interface SubmissionStatic {
  findByUser(user: UserModel): Promise<SubmissionModel[]>;
}

class SubmissionStatic {
  static async findByUser(user: UserModel) {
    return await Submission.find({ user_id: user._id });
  }
}

export class SubmissionClass {
  _id: string;
  user_id: string;
  language: Language;
  code: string;
  createdAt?: Date;
  async User(): Promise<UserModel> {
    return await User.findOne({_id: this.user_id});
  }
  get programName() {
    return `${this._id}.${LanguageMeta[this.language]['extension']}`;
  }
  get compileCommand() {
    return `g++ -o ${this._id} ${this.programName} bot.cpp map.cpp cell.cpp insect.cpp bee.cpp queen_bee.cpp`
  }
  get execCommand() {
    return LanguageMeta[this.language]['execCommand'](this._id);
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
