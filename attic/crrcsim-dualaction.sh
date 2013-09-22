# This script starts with all parameters needed to have good control of a 4 channel plane 
# using a Logitech Dualaction joystick (with throttle on 2 joystick buttons and axis 4)

cd $( dirname $0 ) # make sure we can find crrcsim.cfg
./crrcsim \
  -i JOYSTICK      \
  -j 0:RUDDER      \
  -j 1:ELEVATOR    \
  -j 2:AILERON     \
  -j 3:THROTTLE    \
  -b 1:DECTHROTTLE \
  -b 3:INCTHROTTLE \
  -b 2:RESET       \
  -b 4:ZOOMIN      \
  -b 5:ZOOMOUT     \
  -b 8:PAUSE       \
  -b 9:RESUME      \
  $*
