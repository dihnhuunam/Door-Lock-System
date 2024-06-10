import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import UnlockScreen from './app/screens/UnlockScreen1';
import ChangePasswordScreen from './app/screens/ChangePasswordScreen';

const Stack = createStackNavigator();

const App = () => {
  return (
    <NavigationContainer>
      <Stack.Navigator initialRouteName="UnlockScreen">
        <Stack.Screen name="UnlockScreen" component={UnlockScreen} options={{ title: 'Unlock Door' }} />
        <Stack.Screen name="ChangePasswordScreen" component={ChangePasswordScreen} options={{ title: 'Change Password' }} />
      </Stack.Navigator>
    </NavigationContainer>
  );
};

export default App;
