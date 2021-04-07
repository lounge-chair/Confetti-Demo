# Confetti Demo
## Overview
Confetti Demo is an embedded C program that showcases the core features of my G8RTOS implementation, as well as my HY28B (Touchscreen TFT LCD) driver.

The program launches with nothing on the screen, waiting for a touch. Once touched, a confetto will be drawn on the screen with a random color. The confetti will move based on the values read from the accelerometer on the TI Sensor BoosterPack. To make it more interesting, every new confetto created has a scaling factor for its velocity. If one of the confetti is touched, the confetto is deleted. There is a max number of 20 confetti allowed at one time. If a confetto hits an edge, it wraps around to the other side. 

Each confetto is managed by its own thread in my RTOS. A new confetto thread is added to the scheduler when a touch is registered, and the thread is killed when its associated confetto is touched. In this way, I demonstrate both the scheduling features of my RTOS and its capability for mutual exclusion, as there is a LCD mutex that each thread must acquire before it can write to the LCD screen.

![demo](https://user-images.githubusercontent.com/36556993/113928759-35134300-97bd-11eb-8719-9faf6d3f7dd2.gif)
