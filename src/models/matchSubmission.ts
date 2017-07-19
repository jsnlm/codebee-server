import * as mongoose from 'mongoose';

import logger from '../env/debug';
import { Document, Schema } from 'mongoose';
import { Submission, SubmissionModel } from './submission';

const debug = logger('Match Submission');

export class MatchSubmissionClass {
  constructor (submission: SubmissionModel) {
    this.submission = submission;
  }
  submission: SubmissionModel;
  score: number;
}

export type MatchSubmissionModel = MatchSubmissionClass & Document; //TODO do I need this??

export const MatchSubmissionSchema = new Schema({
  submission: { type: Schema.Types.ObjectId, ref: 'Submission' },
  score: Date,
}, {
  _id: false
});

MatchSubmissionSchema.set('toJSON', {
  id: true,
  getters: true,
  versionKey: false,
  transform: (_: any, ret: Document) => {
    delete ret._id;
    return ret;
  }
});

MatchSubmissionSchema.loadClass(MatchSubmissionClass);

export default MatchSubmissionSchema;
