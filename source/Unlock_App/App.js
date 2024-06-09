import React from 'react';
import { StyleSheet, View } from 'react-native';
import UnlockScreen from './app/views/UnlockScreen';

const App = () => {
  return (
    <View style={styles.container}>
      <UnlockScreen />
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#F8F9FA',
  },
});

export default App;
