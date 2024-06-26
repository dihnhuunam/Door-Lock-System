import React, { useState } from "react";
import {
  View,
  Text,
  TextInput,
  TouchableOpacity,
  StyleSheet,
  StatusBar,
  SafeAreaView,
  TouchableWithoutFeedback,
  Keyboard,
} from "react-native";
import { handleUnlock } from "../controllers/unlockController";

const UnlockScreen = ({ navigation }) => {
  const [password, setPassword] = useState("");
  const [responseMessage, setResponseMessage] = useState("");
  const [responseColor, setResponseColor] = useState("");

  const unlock = () => {
    handleUnlock(password, setResponseMessage, setResponseColor);
  };

  return (
    <TouchableWithoutFeedback onPress={Keyboard.dismiss}>
      <SafeAreaView style={styles.safeArea}>
        <StatusBar barStyle="dark-content" backgroundColor="#FFFFFF" />
        <View style={styles.container}>
          <Text style={styles.title}>Unlock Door</Text>
          <TextInput
            style={styles.input}
            value={password}
            onChangeText={setPassword}
            secureTextEntry
            placeholder="Enter Password"
            keyboardType="numeric"
          />
          <TouchableOpacity style={styles.button} onPress={unlock}>
            <Text style={styles.buttonText}>Unlock</Text>
          </TouchableOpacity>
          {responseMessage ? (
            <Text style={[styles.response, { color: responseColor }]}>
              {responseMessage}
            </Text>
          ) : null}
        </View>
      </SafeAreaView>
    </TouchableWithoutFeedback>
  );
};

const styles = StyleSheet.create({
  safeArea: {
    flex: 1,
    backgroundColor: "#FFFFFF",
  },
  container: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
    padding: 20,
  },
  title: {
    fontSize: 26,
    marginBottom: 30,
    color: "#333333",
    fontWeight: "bold",
  },
  input: {
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
  button: {
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
  buttonText: {
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
