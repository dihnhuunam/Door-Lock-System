import axios from "axios";

const baseUrl = "http://localhost:8000";

export const handleUnlock = async (password, setResponseMessage, setResponseColor) => {
  try {
    const response = await axios.post(
      `${baseUrl}/unlock`,
      { password },
      {
        headers: {
          "Content-Type": "application/json",
        },
      }
    );

    if (response.status === 200) {
      setResponseMessage("Password is correct, door unlocked");
      setResponseColor("green");
    } else {
      setResponseMessage("Password is incorrect");
      setResponseColor("red");
    }
  } catch (error) {
    setResponseMessage("Network error or password is incorrect");
    setResponseColor("red");
  }
};

export const changePassword = async (oldPassword, newPassword, setResponseMessage, setResponseColor) => {
  try {
    const response = await axios.post(
      `${baseUrl}/changePassword`,
      { oldPassword, newPassword },
      {
        headers: {
          "Content-Type": "application/json",
        },
      }
    );

    if (response.status === 200) {
      setResponseMessage("Password changed successfully");
      setResponseColor("green");
    } else {
      setResponseMessage("Failed to change password");
      setResponseColor("red");
    }
  } catch (error) {
    setResponseMessage("Network error or failed to change password");
    setResponseColor("red");
  }
};
