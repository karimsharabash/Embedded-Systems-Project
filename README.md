Embedded-Systems-Project
========================

Embedded systems project at the University of Michigan

1. Project Name: TiM Returning
2. Date: April 21st 2009
3. Group Members: Kyle Egnor, Niranjan Ramadas, Michael Starch
4. Member Contributions:

    Kyle Egnor:
    QADC
  
    Niranjan Ramadas:
    Built Robot chasse
    Ultrasound noise control
    Servos

    Michael Starch:
    Interrupts in Flash and SRAM working
    Final wiring

    Tasks worked out as a group:
    Speaker
    Ultra Sound sensors
    IR integration
    Movement algorithm 
    ISR functions
    GPIO 
    Timings
    Major debugging
    Code Warrior Issues
   * Essentially, this was a group project. The team met together and worked together.
   
5. Percentage Contributions:
    Kyle Egnor: 33 %
    Niranjan Ramadas: 33%
    Michael Starch: 33%
    TiM the Robot: 1%

Introduction
============

Our project was to build a mobile device that retreats from movement. The device recognizes and exhibits object avoidance, in order to not run into things, but does not respond unless an object is moving towards it.
High-Level Design

The robot detects movement via a pair of ultrasonic sensors and detects imminent collisions via infrared sensors. Our ultrasonic sensors are mounted on a plate that scans, looking for movement. When movement is detected, our device moves in the opposite direction of that movement. During all of this, our device outputs different songs from its built-in speaker depending on what action it is currently taking.

Hardware Design
===============

Since we used the MPC555 to control our device, and the 555 does not have an FPGA, our hardware design consisted of just constructing the robot itself. TiM’s chasse was built from foam board and lots of hot glue. Aside from the processor, servos, sensors, and speaker, we also attached a couple bread boards in an attempt to neaten the mass of wires required to make TiM tick. On one of the bread boards, we attached an inverter chip, which we used to trigger interrupts on the falling edge of a pulse rather than the rising edge. The other board held a 5V power regulator, so the 7.2V batteries we were using wouldn’t fry our servos, as well as a capacitor circuit to cut down on noise from the servos.

We mounted the infrared sensors on the front of our robot, angled inwards so they could detect objects that are both immediately in front of the robot and off to one side or the other. These sensors do not require anything more than a constant voltage, and they continuously send back analog data corresponding to how far away the object they detect is.

The ultrasound sensors were mounted on a plate attached to a servo. For as long as these sensors receive a ‘1’ as their control signal, they will range, or send out ultrasound signals one after the other continuously. Their output is a pulse of varying length, which has a rising edge indicating when the ultrasonic signal was send out and a falling edge indicating when it returned. The longer the output pulse, the farther away the object is.

We used two different types of servos: continuous motion servos for the wheels and a normal servo for the ultrasound sensor plate. The servos worked by giving them an input pulse with a specific width that corresponded to how far you wanted the servo to spin. Normal servos spin to the spot the pulse width specifies and then stop and wait for another pulse. However, due to tampering with their potentiometers, the continuous motion servos just keep spinning in the direction of the place you told the servo to spin to.

Software Design
===============

Here is where we spent the majority of our time on this project. Our software was written mostly in C, though our branch table and interrupt handling routines were written in assembly.

We coded our project to work like a finite state machine, controlled by a global variable we call ‘move’. Essentially every part of our software uses interrupts to switch from one state to another. An interrupt will be triggered when the ultrasound sensors detected something coming toward them fast enough and for a long enough period of time for them to notice, and our robot will switch from his ‘stationary’ state to his ‘turning’ state. When TiM is running away from movement, the IR sensors will trigger an interrupt if they detect something within a certain distance away from them, and switch TiM from his ‘moving’ state to his ‘turning’ state. Interrupts are also used in conjunction with the pulse sent out by the ultrasound sensors to determine the relative distance from our robot to whatever object it is detecting. By triggering an interrupt on both the rising edge and falling edge of the ultrasound pulse, we can measure the time between the two and compare that value with previous values to determine whether or not the object is getting closer.

The software we use to play music with the speaker is fairly straightforward. We use PWM to create square waves of various frequencies, and use global arrays to switch between different frequencies to create music. Each time we switch states within our overarching finite state machine, we switch music.

Results
=======

We had a lot of problems getting the ultrasound sensors to work correctly, because they were very sensitive. The slightest amount of ultrasonic noise caused drastically incorrect readings from the sensors. This was taken care of when we implemented various noise-reducing algorithms in our software, and when we ensured the power to the sensors was both securely connected and running through a capacitor circuit to cut down noise.

Our movement algorithm also took quite a few tries to get working well. Initially, we were stopping and scanning in four set spots with our ultrasound sensors, and holding onto the values until we passed by again to compare for distance change. Unfortunately, because of the high chance of glitches in our sensor readings, our robot would see movement when there was none, or, after eliminating noise, it would see no movement when there was some, and we had to scrap that algorithm. Instead, we changed to an algorithm that has our sensors stopping and collecting a large chunk of data in each scanning location, to rule out the chance of glitches ruining everything.

In the end, TiM works quite well for us. He successfully runs away from someone chasing him, as long as there is a big enough surface for the ultrasound to bounce off of, and as long as the person isn’t approaching too slow or too fast. TiM is quite adept at avoiding walls and obstacles as well, successfully turning away from walls and the like every time.
