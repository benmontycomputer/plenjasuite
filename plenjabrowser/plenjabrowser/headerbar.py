import gi
import os

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Vte
from gi.repository import GLib
from gi.repository import Gdk

def makeheaderbar(self):
    self.headerbar = Gtk.HeaderBar()

    #self.headerbar.props.title = "The PlenjaNet"
    self.urlbar = Gtk.Entry()
    self.urlbar.set_hexpand(True)
    #self.urlbar.set_size_request(480, self.urlbar.get_preferred_height()[0])
    self.headerbar.set_custom_title(self.urlbar)

    settingsbutton = Gtk.Button()
    settingsicon = Gtk.Image.new_from_icon_name("open-menu-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
    settingsbutton.add(settingsicon)
    settingsbutton.set_can_focus(False)

    self.headerbar.pack_end(settingsbutton)

    self.optionsmenu1 = Gtk.Menu()

    menu_item = Gtk.MenuItem(label="App Store")
    self.optionsmenu1.append(menu_item)
    menu_item.show()

    settingsbutton.connect_object("button-press-event", self.button_press, self.optionsmenu1)

    newtabbutton = Gtk.Button()
    newtabicon = Gtk.Image.new_from_icon_name("list-add-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
    newtabbutton.add(newtabicon)
    
    gobutton = Gtk.Button(label='Go')
    
    self.headerbar.pack_end(newtabbutton)
    self.headerbar.pack_end(gobutton)

    newtabbutton.connect("clicked", self.new_tab)
    gobutton.connect("clicked", self.go)

    newtabbutton.set_can_focus(False)
    gobutton.set_can_focus(False)

    self.leftbox = Gtk.HBox()
    self.leftbox.props.spacing = 0
    
    self.backbutton = Gtk.Button()
    backarrow = Gtk.Image.new_from_icon_name("go-previous-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
    self.backbutton.add(backarrow)
    self.leftbox.pack_start(self.backbutton, False, False, 0)
    self.backbutton.set_can_focus(False)
    self.backbutton.set_margin_start(0)
    self.backbutton.set_margin_end(0)

    self.forwardbutton = Gtk.Button()
    forwardarrow = Gtk.Image.new_from_icon_name("go-next-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
    self.forwardbutton.add(forwardarrow)
    self.leftbox.pack_start(self.forwardbutton, False, False, 0)
    self.forwardbutton.set_can_focus(False)
    self.forwardbutton.set_margin_start(0)
    self.forwardbutton.set_margin_end(0)

    self.refreshbutton = Gtk.Button()
    refreshicon = Gtk.Image.new_from_icon_name("view-refresh-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
    self.refreshbutton.add(refreshicon)
    self.leftbox.pack_start(self.refreshbutton, False, False, 0)
    self.refreshbutton.set_can_focus(False)
    self.refreshbutton.set_margin_start(4)
    self.refreshbutton.set_margin_end(0)

    self.headerbar.pack_start(self.leftbox)

    self.backbutton.connect("clicked", self.back)
    self.forwardbutton.connect("clicked", self.forward)
    self.refreshbutton.connect("clicked", self.refresh)
    
    self.headerbar.set_show_close_button(True)

    self.headerbar.show_all()

    self.urlbar.set_can_focus(True)

    self.urlbar.set_icon_from_icon_name(Gtk.EntryIconPosition.PRIMARY, "system-search-symbolic")

    self.urlbar.connect("activate", self.go)
def button_press(self, widget, event):
    if event.type == Gdk.EventType.BUTTON_PRESS and event.button == 1:
        #make widget popup
        widget.popup(None, None, None, event.button, event.time, event.time)
        pass
