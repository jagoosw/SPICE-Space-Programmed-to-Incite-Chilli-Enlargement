# SPICE-Space Programmed to Incite Chilli Enlargement
## The Plan
The optimal conditions to grow chillis in seems to be 25 to 30 degrees centigrade and ~60% humidity. The aim of SPICE is to provide this for my chilli plant in a draughty and not very well insulated room. SPICE will consist of a container (probably made from some kind of clear plastic sheet and 3d printed parts) and an enviromental control system that can warm and humidify the plant. Automated watering may also be added at some point in the future.

As well as being here to care for my chilli I am also doing this to learn more about control theory/programming and to practice system design.

Also, any suggestions for a better name are welcome.

## The Enviromental Control
The enviromentl control system consists of an Arduino Nano with a DHT22 sensor. I have then implimented a parallel PID control loop to maintain the temperature at the correct values for day and night time (discussion below). The PID loop was initially tuned by the Ziegler–Nichols method but this may be upgraded to some kind of software tuning as I become more frustrated waiting for it to tune.

Update: lighting has now been added. If the pot has diameter 18cm and the required light intensity is as bellow then we need ~500 lumens of light. I have some 250 lumen LEDs so am using them with another of the led drivers.

The next step is to work out a way to change the humidity of the enclosure. 

As it stands the electronics consist of:
- Arduino Nano Every
- Jumper cables
- DHT22
- SSR
- Switch
- Mains plug and cable
- 240v pipe heater (? not sure what this is called, it is designed to go around a metal pipe I think. Will be adding some kind of heatsink inside)
- 24v 4040 fan
- 2x12v LED driver (wrong voltage for the fan but works)
- Rocker switch for mains circut
- 2x250 lumen LEDs
- 6 long kabab sticks
- SSD1306 screen 

Wiring diagram to follow but temp sensor attatches to D3, GND and power on the arduino. The relay is then attatched over D2 and GND on the switch side and the other side in the live wire of the mains circute. The mains cable is attactched to the LED driver and the heater in parallel and the fan connected to the driver. The rocker switch is in series with the SSR. Another LED driver is attatched directly to the mains wiring and the two LEDs in parallel to the driver.

## The Container
Origionally the plan was to make a cubic container out of acrylic. When I discovered that acrylic would cost ~10x more than the plant cost I immediatly scrapped the idea. The container I have settled on is a clear plastic bag with a frame to hold it off the top of the plant with a lid to allow it to be watered. 

All of the parts can be printed in PLA/PETG except for the heater duct which **must** be printed in PC otherwise it will melt.

The design also needs 3xM5, 4xM3x16 nut and bolts and a large clear bag.

## Ideal Conditionss
From this [paper](https://academicjournals.org/journal/JHF/article-full-text-pdf/73E1C1341288) we can see from their limited range of contitions that chillis grew best in the winter in the shade. This meant that they were at an average of 30.1 degrees centigrade, 67.1% humidity and 18333.74 lux light intensity.

This [website](https://www.thompson-morgan.com/how-to-grow-chilli-peppers) suggests that when they are small chillis should be kept between 18 and 25 degrees centigrade. This [one](https://www.lovethegarden.com/uk-en/article/how-grow-chillies) suggests 18 to 21 degrees centigrade. This [one](https://verticalveg.org.uk/how-to-grow-chillies-in-containers/) 25 to 30 degrees centigrade. This [one](https://www.southdevonchillifarm.co.uk/how-to/growing-chilli-plants/) 27 to 32 degrees but less than 36 degrees when larger. 

This [page](https://www.rhs.org.uk/advice/profile?pid=664) suggests that 15 degrees as a minimum at night is best and less than 30 degrees during the day. This [one](https://www.greenhousesensation.co.uk/over-wintering-chilli-plants/) 21 degrees during growing season (but they don't want them to grow over winter but I don't know why).

This [instructable](https://www.instructables.com/id/12-Tips-on-Growing-Hot-Chilli-Peppers-in-a-Cold-Cl/) says 27 to 32 degrees and that stability is important. They also suggest that you should only water with warm water. 

There seems to be a lot of disagreement but I will go with 30 degrees during the day and 20 degrees at night.