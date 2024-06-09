import React from 'react';
import { TouchableOpacity, Text, StyleSheet } from 'react-native';

const Key = ({ digit, onPress }) => {
  return (
    <TouchableOpacity style={styles.key} onPress={onPress}>
      <Text style={styles.keyText}>{digit}</Text>
    </TouchableOpacity>
  );
};

const styles = StyleSheet.create({
  key: {
    width: '30%',
    padding: 15,
    marginVertical: 5,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#E0E0E0',
    borderWidth: 1,
    borderColor: '#BDBDBD',
    borderRadius: 10,
    shadowColor: '#000',
    shadowOpacity: 0.1,
    shadowOffset: { width: 0, height: 1 },
    shadowRadius: 2,
  },
  keyText: {
    fontSize: 20,
    color: '#333333',
  },
});

export default Key;
