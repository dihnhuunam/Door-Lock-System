import { unlockDoor, changeUserPassword } from "../models/unlockModel";

export const handleUnlock = async (
  password,
  setResponseMessage,
  setResponseColor
) => {
  try {
    const status = await unlockDoor(password);

    if (status === 200) {
      setResponseMessage("Door unlocked successfully!");
      setResponseColor("green");
    } else if (status === 401) {
      setResponseMessage("Incorrect password. Please try again.");
      setResponseColor("red");
    } else {
      setResponseMessage("An error occurred. Please try again.");
      setResponseColor("red");
    }
  } catch {
    setResponseMessage("An error occurred. Please try again.");
    setResponseColor("red");
  }
};

export const changePassword = async (
  currentPassword,
  newPassword,
  setResponseMessage,
  setResponseColor
) => {
  try {
    const status = await changeUserPassword(currentPassword, newPassword);

    if (status === 200) {
      setResponseMessage("Password changed successfully!");
      setResponseColor("green");
    } else if (status === 401) {
      setResponseMessage("Incorrect current password. Please try again.");
      setResponseColor("red");
    } else {
      setResponseMessage("An error occurred. Please try again.");
      setResponseColor("red");
    }
  } catch {
    setResponseMessage("An error occurred. Please try again.");
    setResponseColor("red");
  }
};
