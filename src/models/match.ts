import * as mongoose from 'mongoose';

import logger from '../env/debug';
import { Document, Model, Schema } from 'mongoose';
import { UserModel } from './user';
import { MatchSubmissionClass, MatchSubmissionSchema } from './matchSubmission';
import { Submission, SubmissionModel } from './submission';

const debug = logger('Match');

export enum MatchState {
    NotRun = 'Not Run',
    Running = 'Running',
    Finished = 'Finished',
    CompilationError = 'Compilation Error',
    RuntimeError = 'Runtime Error',
};

interface MatchStatic {
  findBySubmission(submission: SubmissionModel): Promise<MatchModel[]>;
  findByUser(user: UserModel): Promise<MatchModel[]>;
}

class MatchStatic {
  static async findBySubmission(submission: SubmissionModel) {
    return await Match.find({submissions: {submission: submission._id}});
  }
  
  static async findByUser(user: UserModel) {
    let _submissions: SubmissionModel[] = await Submission.findByUser(user);
    return await Match.find({"submissions.submission": {$in: _submissions.map(sub => sub._id)}});
  }
}

export class MatchClass {
  submissions: MatchSubmissionClass[];
  replayFile?: string;
  matchTime: Date;
  turns: number;
  state: MatchState;
  addSubmission(submission: SubmissionModel) {
    this.submissions = this.submissions || [];
    this.submissions.push(new MatchSubmissionClass(submission));
  }
  addSubmissions(submissions: SubmissionModel[]) {
    submissions.forEach(submission => this.addSubmission(submission));
  }
}

export type MatchModel = MatchClass & Document;
type MatchType = MatchClass & MatchStatic & Model<MatchModel>;

const schema = new Schema({
  submissions: [MatchSubmissionSchema],
  replayFile: String,
  matchTime: Date,
  turns: Number,
  state: String,
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

schema.loadClass(MatchClass);
schema.loadClass(MatchStatic);

export const Match = mongoose.model<MatchModel>('Match', schema) as MatchType;

export default Match;
