import * as casual from 'casual';

import { UserModel } from '../models/user';
import { Submission, SubmissionClass, SubmissionModel } from '../models/submission';
import { Match, MatchClass, MatchModel } from '../models/match';
import { seedSubmissions } from './submission.seed';

casual.define('matchState', ():string => {
  let states =['Not Run', 'Running', 'Finished', 'Compilation Error', 'Runtime Error'];
  return states[Math.floor((Math.random() * states.length))];
});

casual.define('MatchClass', ():SubmissionClass => { 
  return {
    replayFile: casual.text,
    matchTime: new Date(casual.unix_time),
    turns: casual.integer(2, 20), //random number from 2-20
    state: (casual as any).matchState,
  } as any
});

export async function seedMatch(submissions?: SubmissionModel[], winner?: SubmissionModel, presets = {}): Promise<MatchModel> {
  if (!submissions) {
    submissions = submissions || await seedSubmissions(2);
    winner = submissions[0];
  }
  winner = winner || submissions[0];

  let match:MatchClass = (casual as any).MatchClass;
  match.submission_ids = submissions.map((sub) => sub._id);
  match.winner_id = winner._id;
  return await Match.create(Object.assign(match, presets));
}

export async function seedMatches(count: number, submissions?: SubmissionModel[], winner?: SubmissionModel, presets = {}): Promise<MatchModel[]> {
  if (!submissions) {
    submissions = submissions || await seedSubmissions(2);
    winner = submissions[0];
  }
  winner = winner || submissions[0];
  
  const matches: MatchModel[] = [];
  for (let i = 0; i < count; i++) {
    matches.push(await seedMatch(submissions, winner, presets));
  }
  return matches;
}
