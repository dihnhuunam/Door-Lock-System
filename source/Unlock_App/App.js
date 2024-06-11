import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import UnlockScreen from './app/screens/UnlockScreen';
import ChangePasswordScreen from './app/screens/ChangePasswordScreen';
import AddPasswordScreen from './app/screens/AddPasswordScreen';
import HomeScreen from './app/screens/HomeScreen';

const Stack = createStackNavigator();

const App = () => {
  return (
    <NavigationContainer>
      <Stack.Navigator initialRouteName="HomeScreen">
        <Stack.Screen name="HomeScreen" component={HomeScreen} options={{ title: 'Home' }} />
        <Stack.Screen name="UnlockScreen" component={UnlockScreen} options={{ title: 'Unlock Door' }} />
        <Stack.Screen name="ChangePasswordScreen" component={ChangePasswordScreen} options={{ title: 'Change Password' }} />
        <Stack.Screen name="AddPasswordScreen" component={AddPasswordScreen} options={{ title: 'Add Password' }} />
      </Stack.Navigator>
    </NavigationContainer>
  );
};

export default App;
