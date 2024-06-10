import React from 'react';
import { View, StyleSheet } from 'react-native';
import Key from './Key';

const Keypad = ({ addDigit, clearPassword, deleteLastDigit }) => {
  const keys = [
    '1', '2', '3', 
    '4', '5', '6', 
    '7', '8', '9', 
    'Clear', '0', 'Delete'
  ];

  return (
    <View style={styles.keypad}>
      {keys.map((digit) => (
        <Key key={digit} digit={digit} onPress={() => {
          if (digit === 'Clear') {
            clearPassword();
          } else if (digit === 'Delete') {
            deleteLastDigit();
          } else {
            addDigit(digit);
          }
        }} />
      ))}
    </View>
  );
};

const styles = StyleSheet.create({
  keypad: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-between',
    width: '80%',
  },
});

export default Keypad;
