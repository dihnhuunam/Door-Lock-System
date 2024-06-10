import React, { useState } from "react";
import {
  View,
  Text,
  TextInput,
  TouchableOpacity,
  StyleSheet,
} from "react-native";
import Keypad from "../components/Keypad";
import { handleUnlock } from "../controllers/unlockController";

const UnlockScreen = ({ navigation }) => {
  const [password, setPassword] = useState("");
  const [responseMessage, setResponseMessage] = useState("");
  const [responseColor, setResponseColor] = useState("");

  const addDigit = (digit) => {
    setPassword(password + digit);
  };

  const clearPassword = () => {
    setPassword("");
  };

  const deleteLastDigit = () => {
    setPassword(password.slice(0, -1));
  };

  const unlock = () => {
    handleUnlock(password, setResponseMessage, setResponseColor);
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Unlock Door</Text>
      <TextInput
        style={styles.passwordInput}
        value={password}
        secureTextEntry
        editable={false}
      />
      <Keypad
        addDigit={addDigit}
        clearPassword={clearPassword}
        deleteLastDigit={deleteLastDigit}
      />
      <TouchableOpacity style={styles.unlockButton} onPress={unlock}>
        <Text style={styles.unlockButtonText}>Unlock</Text>
      </TouchableOpacity>
      <TouchableOpacity
        style={styles.changePasswordButton}
        onPress={() => navigation.navigate("ChangePasswordScreen")}
      >
        <Text style={styles.changePasswordButtonText}>Change Password</Text>
      </TouchableOpacity>
      {responseMessage ? (
        <Text style={[styles.response, { color: responseColor }]}>
          {responseMessage}
        </Text>
      ) : null}
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
    padding: 20,
    backgroundColor: "#FFFFFF",
  },
  title: {
    fontSize: 26,
    marginBottom: 30,
    color: "#333333",
    fontWeight: "bold",
  },
  passwordInput: {
    width: "80%",
    padding: 15,
    marginBottom: 20,
    borderWidth: 1,
    borderColor: "#DDDDDD",
    borderRadius: 10,
    fontSize: 18,
    backgroundColor: "#F8F9FA",
    color: "#333333",
  },
  unlockButton: {
    width: "80%",
    padding: 15,
    borderRadius: 10,
    backgroundColor: "#007BFF",
    justifyContent: "center",
    alignItems: "center",
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.2,
    shadowRadius: 4,
    marginTop: 20,
  },
  unlockButtonText: {
    fontSize: 18,
    color: "#FFFFFF",
    fontWeight: "bold",
  },
  changePasswordButton: {
    width: "80%",
    padding: 15,
    borderRadius: 10,
    backgroundColor: "#6C757D",
    justifyContent: "center",
    alignItems: "center",
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.2,
    shadowRadius: 4,
    marginTop: 20,
  },
  changePasswordButtonText: {
    fontSize: 18,
    color: "#FFFFFF",
    fontWeight: "bold",
  },
  response: {
    marginTop: 20,
    fontSize: 16,
    textAlign: "center",
  },
});

export default UnlockScreen;



