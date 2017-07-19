import * as mongoose from 'mongoose';

import logger from '../env/debug';
import { Document, Model, Schema } from 'mongoose';
import { UserModel } from './user';
import { Submission, SubmissionModel } from './submission';

const debug = logger('Simulation');

export enum MatchState {
    NotRun = 'Not Run',
    Running = 'Running',
    Finished = 'Finished',
    CompilationError = 'Compilation Error',
    RuntimeError = 'Runtime Error',
};

export class MatchSubmission {
  submission_id: string;
  score: number;
  async SubmissionModel(): Promise<SubmissionModel> {
    debug('SubmissionModel()');
    return await Submission.findById(this.submission_id);
  }
}

interface MatchStatic {
  findBySubmission(submission: SubmissionModel): Promise<MatchModel[]>;
  findByUser(user: UserModel): Promise<MatchModel[]>;
}

class MatchStatic {
  static async findBySubmission(submission: SubmissionModel) {
    return Match.find({submission_ids: submission._id});
  }
  
  static async findByUser(user: UserModel) {
    let submissions: SubmissionModel[] = await Submission.findByUser(user);
    return Match.find({submission_ids: {$in: submissions}});
  }
}

export class MatchClass {
  submissions: MatchSubmission[];
  replayFile?: string;
  matchTime: Date;
  turns: number;
  state: MatchState;
  async addSubmission(submission: SubmissionModel) {
    this.submissions = this.submissions || [];

    let matchSubmission = new MatchSubmission();
    matchSubmission.submission_id = submission._id;
    matchSubmission.score = 0;
    this.submissions.push(matchSubmission);
  }
  async addSubmissions(submissions: SubmissionModel[]) {
    submissions.forEach(submission => this.addSubmission(submission));
  }
}

export type MatchModel = MatchClass & Document;
type MatchType = MatchClass & MatchStatic & Model<MatchModel>;

const schema = new Schema({
  submissions: [
    {
      submission_id: Schema.Types.ObjectId,
      score: Number,
    }
  ],
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
