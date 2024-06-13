import { Platform } from 'react-native';

let config;

if (Platform.OS === 'android' || Platform.OS === 'ios') {
  config = {
    PROTOCOL: 'http',
    IP: '192.168.1.99',
    PORT: '8000',
  };
} else {
  config = {
    PROTOCOL: process.env.PROTOCOL,
    IP: process.env.IP,
    PORT: process.env.PORT,
  };
}

const baseUrl = `${config.PROTOCOL}://${config.IP}:${config.PORT}`;

export default baseUrl;
