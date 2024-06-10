import axios from "axios";

export const unlockDoor = async (password) => {
  const url = "http://localhost:8000/unlock";
  try {
    const response = await axios.post(
      url,
      `password=${encodeURIComponent(password)}`,
      {
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
        },
      }
    );

    return response.status;
  } catch (error) {
    throw new Error("Network error");
  }
};

export const changeUserPassword = async (currentPassword, newPassword) => {
  const url = "http://localhost:8000/change-password";
  try {
    const response = await axios.post(
      url,
      `currentPassword=${encodeURIComponent(
        currentPassword
      )}&newPassword=${encodeURIComponent(newPassword)}`,
      {
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
        },
      }
    );

    return response.status;
  } catch (error) {
    throw new Error("Network error");
  }
};
