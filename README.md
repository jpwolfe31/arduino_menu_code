# Arduino-Menu-Code
Simple easy to understand full menu system for arduino.  No library required.

![menu_display](https://github.com/user-attachments/assets/f8289c96-ee5c-4a30-b8b3-e41b31a63f7f)

I wanted a simple full menu system for my arduino projects.  I asked CoPilot to help and they provided a few examples.  I took the easiest one and added a couple of nice to haves and uploaded examples here.

The first one has four buttons (up down select back) and unlimited nesting and screeens.  The system uses a common data strucuture for each screen.  The screen repositions when the scroll hits the top or bottom.  It works on LCD and OLED displays.  The user will need to tweak the code in the examples for the particular display used.  I used a 64 by 128 i2c OLED display I purchased on AliExpress.  It is also now available on Amazon.  It would be easy to implement on a 2 to 4 line LCD too.

The second example includes a short and long button push to enable the full menu system with just two buttons.

What I like about this code is that it is easy to understand and looks and works beautifully.

Happy to answer any questions.








