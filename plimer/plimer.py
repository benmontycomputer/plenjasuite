import gi
import time
import datetime
import threading
from playsound import playsound #

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk




# yes, 4pm yesterday maybe
class MyWindow(Gtk.Window):
    def __init__(self):
        super().__init__(title="Plimer")

        self.grid = Gtk.Grid()
        
        self.hour = Gtk.SpinButton()
        self.houradj = Gtk.Adjustment(upper=24, step_increment=1, page_increment=10)
        self.hour.set_adjustment(self.houradj)

        self.min = Gtk.SpinButton()
        self.minadj = Gtk.Adjustment(upper=60, step_increment=1, page_increment=10)
        self.min.set_adjustment(self.minadj)

        self.create = Gtk.Button(label="Create")
        self.create.connect("clicked", self.on_create_clicked)
        self.stoptimer = False #yes it did


        self.label = Gtk.Label(label="Time left")
        
        self.grid.add(self.hour)
        self.grid.attach(self.min,0,1,1,1)
        self.grid.attach(self.create,1,0,1,1)
        self.grid.attach(self.label,1,1,1,1)
        self.add(self.grid) #explicame porque no worko
        self.grid.show_all()
        self.set_default_size(240, 80)
        self.hour.set_vexpand(True)
        self.min.set_vexpand(True)
        self.create.set_hexpand(True)
        self.create.set_vexpand(True)
        self.label.set_hexpand(True)
        self.label.set_vexpand(True)
        self.grid.set_hexpand(True)
        self.grid.set_vexpand(True)




    def on_create_clicked(self, widget):
        self.stoptimer = True # i don't like it there muse be another way
        time.sleep(0.1)
        print(self.min.get_value_as_int())

        self.stoptimer = False
        self.timer_thread = threading.Thread(target=self.countdown, name="Timer", args=(self.hour.get_value_as_int(), self.min.get_value_as_int(), 0))
        self.timer_thread.start() #but still no worko

    def countdown(self, h, m, s):
 
        # Calculate the total number of seconds
        total_seconds = h * 3600 + m * 60 + s

        print(total_seconds)
        # While loop that checks if total_seconds reaches zero
        # If not zero, decrement total time by one second
        while total_seconds > 0:
     
            # Timer represents time left on countdown
            timer = datetime.timedelta(seconds = total_seconds) #quid est datetime

            # should i add reset button or does it not matter            
            # Prints the time left on the timer
            print(timer, end="\n")
            self.label.set_text(str(timer))
     
            # Delays the program one se cond
            for i in range(0, 10):
                time.sleep(0.1)
                if (self.stoptimer == True):
                    return
     
            # Reduces total time by one second
            total_seconds -= 1
            if (self.stoptimer == True):
                return

     

        self.label.set_text("Plime Up!")
        playsound('/home/trey/Programs/gobject/plimer/ring.wav') #
        # time for sfx

    def quit(self, arg1):
        self.stoptimer = True
        Gtk.main_quit(self, arg1) # are you ok with that? doing self.timer_thread stop without it existing?

win = MyWindow()
win.connect("destroy", win.quit)
win.show_all()
Gtk.main() 
