import logger from '../../env/debug';

import { setupTest } from '../../test/helper';
import { seedSubmission } from '../../test/submission.seed';
import { Submission, SubmissionClass, SubmissionModel } from '../submission';
import { MatchSubmissionClass } from '../matchSubmission';

const debug = logger('match.test');

let submission: SubmissionModel;
let matchSubmission: MatchSubmissionClass;

beforeAll(async () => {
  await setupTest();
  submission = await seedSubmission();
  matchSubmission = new MatchSubmissionClass(submission);
});

test('test submission reference', () => {
  expect(matchSubmission.submission._id).toEqual(submission._id);
});