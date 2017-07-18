import * as mongoose from 'mongoose';
import { Document, Model, Schema } from 'mongoose';
import { UserModel } from './user';
import { Submission, SubmissionModel } from './submission';

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
    return await Match.find({submission_ids: submission._id});
  }
  
  static async findByUser(user: UserModel) {
    let submissions: SubmissionModel[] = await Submission.findByUser(user);
    return await Match.find({submission_ids: {$in: submissions}});
  }
}

export class MatchClass {
  submission_ids: string[];
  winner_id: string;
  replayFile?: string;
  matchTime: Date;
  turns: Number;
  state: MatchState;
  async Submissions(): Promise<SubmissionModel[]> {
    return await Submission.find({_id: {$in: this.submission_ids}});
  };
}

export type MatchModel = MatchClass & Document;
type MatchType = MatchClass & MatchStatic & Model<MatchModel>;

const schema = new Schema({
  submission_ids: [Schema.Types.ObjectId],
  winner_id: Schema.Types.ObjectId,
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
