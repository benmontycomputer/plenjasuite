#!/usr/bin/python3
#https://python-gtk-3-tutorial.readthedocs.io/en/latest/introduction.html
import gi
import os
#https://stackoverflow.com/questions/18739239/python-how-to-get-stdout-after-running-os-system
import subprocess

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Vte
from gi.repository import GLib

#https://stackoverflow.com/questions/647041/need-a-simple-hello-world-example-using-the-webkit-library-in-python
gi.require_version('WebKit2', '4.0')
from gi.repository import WebKit2


class MainWindow(Gtk.Window):
    def __init__(self):
        super().__init__(title="Software Updates")
        self.set_default_size(720, 480)

        updatecheckbutton = Gtk.Button(label="Check for Updates")
        updatecheckbutton.connect("clicked", self.update_check)
        
        updatebutton = Gtk.Button(label="Install Updates")
        updatebutton.connect("clicked", self.update_install)
        
        grid1 = Gtk.Grid()
        #grid1.set_column_homogeneous(True)
        grid1.set_column_spacing(10)
        #grid1.set_spacing(10)
        grid1.attach(updatecheckbutton, 2, 1, 1, 1)
        grid1.attach(updatebutton, 3, 1, 1, 1)

        self.notebook1 = Gtk.Notebook()
        updatelabel = Gtk.Label(label="Software Updates")
        self.notebook1.append_page(grid1, updatelabel)

        self.appgrid = Gtk.Grid()
        self.appgrid.set_column_homogeneous(True)
        self.appgrid.set_row_homogeneous(True)

        #placeholderlabel = Gtk.Label(label="App store coming soon!")
        #self.appgrid.add(placeholderlabel)

        os.system("wget -O \"${PWD}/gallery\" \"$(sed -n '/^\s*#/!{p;q}' /etc/ppkg-manager/baseurl.conf)/applications/gallery\"")
        #https://stackabuse.com/read-a-file-line-by-line-in-python/#readafilelinebylineinpythonwithreadlines
        
        #with open('gallery', 'r') as gallery:
        #    apps = gallery.readlines()
        #    for i, line in enumerate(apps):
        #        appbutton = Gtk.Button(label=line)
        #        self.appgrid.add(appbutton)
        #    
        #    gallery.close()
        #https://stackoverflow.com/questions/647041/need-a-simple-hello-world-example-using-the-webkit-library-in-python
        webview = WebKit2.WebView()
        
        webview.load_uri("http://" + subprocess.check_output(["echo $(sed -n '/^\s*#/!{p;q}' /etc/ppkg-manager/baseurl.conf)/applications/galleryview.html"], shell=True, text=True))
        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.add(webview)

        self.appgrid.add(scrolled_window)
        
        appstorelabel = Gtk.Label(label="App Store")
        self.notebook1.append_page(self.appgrid, appstorelabel)

        self.add(self.notebook1)
        
        #self.add(grid1)

        self.terminal     = Vte.Terminal()
        self.terminal.spawn_sync(
            Vte.PtyFlags.DEFAULT,
            os.environ['HOME'],
            ["/bin/sh"],
            [],
            GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            None,
            None,
            )


        grid1.attach(self.terminal, 1, 2, 4, 5)
        #listbox1 = Gtk.ListBox()
        
        #listbox1.set_selection_mode(Gtk.SelectionMode.NONE)
        
        #listbox1.add(updatecheckbutton)
        #listbox1.add(updatebutton)
        #row = Gtk.ListBoxRow()
        #row.add(terminal)
        #row.set_vexpand(True)
        #listbox1.set_vexpand(True)
        #listbox1.add(row)
        
        #self.add(listbox1)
        
    def update_check(self, widget):
        #os.system("xfce4-terminal --command=\"pkexec /usr/bin/ppkg-update update && echo 'you can close this window now.'\" -H")
        #os.system("ppkg-update")
        self.terminal.spawn_sync(
            Vte.PtyFlags.DEFAULT,
            os.environ['HOME'],
            ["/bin/sh", "-c", "ppkg-update && sh --login"],
            [],
            GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            None,
            None,
            )
    def update_install(self, widget):
        #os.system("xfce4-terminal --command=\"pkexec /usr/bin/ppkg-update update && echo 'you can close this window now.'\" -H")
        #os.system("ppkg-update")
        self.terminal.spawn_sync(
            Vte.PtyFlags.DEFAULT,
            os.environ['HOME'],
            ["/bin/sh", "-c", "pkexec /usr/bin/ppkg-update update && sh --login"],
            [],
            GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            None,
            None,
            )
    
win = MainWindow()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()
