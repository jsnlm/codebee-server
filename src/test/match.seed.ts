import * as casual from 'casual';

import { UserModel } from '../models/user';
import { Submission, SubmissionClass, SubmissionModel } from '../models/submission';
import { Match, MatchClass, MatchModel, MatchState } from '../models/match';
import { seedSubmissions } from './submission.seed';

casual.define('matchState', ():string => {
  let states =['Not Run', 'Running', 'Finished', 'Compilation Error', 'Runtime Error'];
  return states[Math.floor((Math.random() * states.length))];
});

casual.define('MatchClass', () => { 
  let matchClass = new MatchClass();
  matchClass.replayFile = casual.text;
  matchClass.matchTime = new Date(casual.unix_time);
  matchClass.turns = casual.integer(2, 20), //random number from 2-20
  matchClass.state = (casual as any).matchState;
  return matchClass;
});

export async function seedMatch(submissions?: SubmissionModel[], presets = {}): Promise<MatchModel> {
  submissions = submissions || await seedSubmissions(2);

  let match:MatchClass = (casual as any).MatchClass;
  await match.addSubmissions(submissions);
  if (match.state == MatchState.Finished) { // Add random score if the match was already played
    match.submissions.forEach(submission => {
      submission.score = Math.random();
    });
  }
  let matchModel: MatchModel = await Match.create(Object.assign(match, presets));
  return matchModel;
}

export async function seedMatches(count: number, submissions?: SubmissionModel[], presets = {}): Promise<MatchModel[]> {
  submissions = submissions || await seedSubmissions(2);
  
  const matches: MatchModel[] = [];
  for (let i = 0; i < count; i++) {
    matches.push(await seedMatch(submissions, presets));
  }
  return matches;
}
