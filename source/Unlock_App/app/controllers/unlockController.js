import { unlockDoor } from '../models/unlockModel';

export const handleUnlock = async (password, setResponseMessage, setResponseColor) => {
  try {
    const status = await unlockDoor(password);

    if (status === 200) {
      setResponseMessage('Door unlocked successfully!');
      setResponseColor('green');
    } else if (status === 401) {
      setResponseMessage('Incorrect password. Please try again.');
      setResponseColor('red');
    } else {
      setResponseMessage('An error occurred. Please try again.');
      setResponseColor('red');
    }
  } catch {
    setResponseMessage('An error occurred. Please try again.');
    setResponseColor('red');
  }
};
