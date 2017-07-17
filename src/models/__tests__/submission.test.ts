import { setupTest } from '../../test/helper';
import { seedUser } from '../../test/user.seed';
import { seedSubmissions } from '../../test/submission.seed';
import { User, UserClass, UserModel } from '../user';
import { Submission, SubmissionClass, SubmissionModel } from '../submission';

let user: UserModel;
let submissions: SubmissionModel[];

beforeAll(async () => {
  await setupTest();``
  user = await seedUser();
  submissions = await seedSubmissions(3, user);
});

test('User is set properly', async () => {
  expect((await submissions[0].User())._id).toEqual(user._id);
});

test('Submission can be retreived from user', async () => {
  expect((await Submission.findByUser(user)).length).toEqual(3);
});