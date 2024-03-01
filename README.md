# Smart-Lamp
The project for Lutron Lighting Innovation Competition, Boston University

**This project's goal** is to create a smart device that involves light using simple sensors, reused material for sustainability, and an automated system. 

Smart-Lamb will use the ultrasonic sensor to check the distance between the user and the device. The closer the user gets, the brighter the lamb is. It's the simulation of user detection to make it automatically turn on. 

Moreover, it has a temperature sensor (tmp36) that receives the temperature input, The information regarding temperature and distance will be displayed on LCD. The color of the light will be adjusted based on the environment to make sure that users are comfortable while accommodating. Light blue when it's hot, Warm white when it's warm, and light red when it's cold.

**How to set up**
1. Manufacturing the box and components as indicating in the sketchinh

2. Push the C++ code into Arduino via USB Cable
  You should be able to change the RGB value color and the temperature range in the code

3. Connect the battery 

**Things to improve and develop**
1. Subsitute the ultrasonics sensor with higher quality sensor to detect people or using ZED camera to detect people using Computer Vision Instead

2. Design more balance and higher quality in product design

3. Considering other factors apart from temperature

4. Making it IoT by developing application or website that can check real-time environment from the device via computer networking
