import * as casual from 'casual';

import { UserModel } from '../models/user';
import { Submission, SubmissionClass, SubmissionModel } from '../models/submission';

casual.define('language', ():string => {
  let languages =['javascript', 'python', 'ruby'];
  return languages[Math.floor((Math.random() * languages.length))];
});

casual.define('SubmissionClass', ():SubmissionClass => { 
  return {
    language: (casual as any).language,
    code: 'sample code',
  } as any
});

export async function seedSubmission(user: UserModel, presets = {}): Promise<SubmissionModel> {
  let submission:SubmissionClass = (casual as any).SubmissionClass;
  submission.user_id = user._id;
  return await Submission.create(Object.assign(submission, presets));
}

export async function seedSubmissions(count: number, user: UserModel, presets = {}): Promise<SubmissionModel[]> {
  const submissions: SubmissionModel[] = [];
  for (let i = 0; i < count; i++) {
    submissions.push(await seedSubmission(user, presets));
  }
  return submissions;
}
