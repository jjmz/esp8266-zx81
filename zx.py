import socket
import curses

usock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

addr = ("192.168.1.157",1025)

conv=" ..........\"$$:?()<>=+-*/;,.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

keys = { '/':(0,254), 'z':(0,253), 'x':(0,251), 'c':(0,247), 'v':(0,239),
         'a':(1,254), 's':(1,253), 'd':(1,251), 'f':(1,247), 'g':(1,239),
         'q':(2,254), 'w':(2,253), 'e':(2,251), 'r':(2,247), 't':(2,239),
         '1':(3,254), '2':(3,253), '3':(3,251), '4':(3,247), '5':(3,239),
         '0':(4,254), '9':(4,253), '8':(4,251), '7':(4,247), '6':(4,239),
         'p':(5,254), 'o':(5,253), 'i':(5,251), 'u':(5,247), 'y':(5,239),
         '\n':(6,254),'l':(6,253), 'k':(6,251), 'j':(6,247), 'h':(6,239),
         ' ':(7,254), '.':(7,253), 'm':(7,251), 'n':(7,247), 'b':(7,239) }

curses.initscr()
win=curses.newwin(25,35,0,0)

lcnt=0
shift=0

while 1:

 win.nodelay(1)
 c=win.getch()
 if c == curses.ERR:
   if (lcnt != 0):
     lcnt = lcnt - 1
     if (lcnt == 0):
       data = last+chr(255)
     else:
       data = "G"
   else:
     data = "G"
 elif c == ord('\\'):    # curses.KEY_HOME:
   break
 elif c == ord('+'):  
   data = "F"
 elif c == ord('-'):  
   data = "S"
 elif c == ord('/'):
   if (shift==0):
     data='0'+chr(254)
     shift=1
   else:
     data='0'+chr(255)
     shift=0
 else:
     (line,col)=keys[chr(c)]
     if ((line == 0) and (shift==1)):
       col = col & 254
     data = chr(48+line)+chr(col)
     last = chr(48+line)
     lcnt = 10

 usock.sendto(data,addr)

 data=usock.recvfrom(792)

 i=0
 j=-1

 for x in range(24):
  for y in range(33):
    c = data[0][y+33*x]
    if ( ord(c) == 118 ):
      i=0
      j=j+1
    else:
      if (j == -1):
        j=0
      win.addch(j,i,conv[ord(c)&63],curses.A_REVERSE if (ord(c)&128) else 0)
      i=i+1
      if (i > 32):
        i=0
        j=j+1
    if (j > 23):
      j=0
  win.refresh()

curses.endwin()

