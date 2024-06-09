export const unlockDoor = async (password) => {
  const url = 'http://localhost:8000/';
  try {
    const response = await fetch(url, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
      },
      body: `password=${encodeURIComponent(password)}`,
    });

    return response.status;
  } catch (error) {
    throw new Error('Network error');
  }
};
