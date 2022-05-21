#!/usr/bin/python3
#https://python-gtk-3-tutorial.readthedocs.io/en/latest/introduction.html
#tab drag + drop is a modified version of: https://gist.github.com/ma8ma/9d5fe2e830c722dac87affe7ff3817cb
import gi
import os
import validators

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Vte
from gi.repository import GLib
from gi.repository import Gdk

# use this for production
from plenjabrowser import settings, headerbar

#import headerbar
#import settings

#https://stackoverflow.com/questions/647041/need-a-simple-hello-world-example-using-the-webkit-library-in-python
gi.require_version('WebKit2', '4.0')
from gi.repository import WebKit2

class TabWrapper:
    def paste(self, arg1):
        self.tab1.paste_clipboard()
    def copy(self, arg1):
        self.tab1.copy_clipboard()
    def closetab(self, arg1, arg2 = None):
        self.notebook.remove(self.tab1)
        if self.notebook.get_nth_page(0) == None:
            self.notebook.parentwindow.new_tab(self.notebook.parentwindow)

class NotebookWrapper:
    def button_press(self, widget, event):
        if event.type == Gdk.EventType.BUTTON_PRESS and event.button == 3 and self.notebook.get_nth_page(0) == None:           #make widget popup
            widget.popup(None, None, None, event.button, event.time, event.time)
            pass

class MainWindow(Gtk.Window):
    notebook1 = Gtk.Notebook()
    def __init__(self):
        super().__init__()

        self.browsersettings = settings.browsersettings()
        
        self.set_default_size(960, 720)
        
        self.notebook1 = Gtk.Notebook()
        self.add(self.notebook1)

        self.notebook1.set_hexpand(True)
        self.notebook1.set_vexpand(True)

        self.notebook1.set_scrollable(True)

        self.notebook1.parentwindow = self
        #self.notebook1.connect('create-window', self.notebook_create_window)
        self.notebook1.set_group_name('0')

        #self.notebook1.connect_after('drag-begin', self._sig_notebook_drag_begin)
        #self.notebook1.connect_after('drag-data-get', self._drag_data_get)

        #self.notebook1.connect('drag-drop', self._sig_drag_drop)
        #self.notebook1.connect_after('drag-data-received', self._drag_data_received)
        self.notebook1.drag_dest_set(Gtk.DestDefaults.HIGHLIGHT,
                         [Gtk.TargetEntry.new('GTK_NOTEBOOK_TAB',
                                              Gtk.TargetFlags.SAME_APP, 0),
                          Gtk.TargetEntry.new('GTK_NOTEBOOK_TAB',
                                              Gtk.TargetFlags.OTHER_APP, 0)],
        #                  ],
                         Gdk.DragAction.MOVE)

        self.new_tab(self)

        self.show_all()

        self.notebook1.connect('key-press-event', self.on_key_press)

        headerbar.makeheaderbar(self)

        self.set_titlebar(self.headerbar)
    
    drag_page_number = 0
    def go(self, widget):
        url = self.urlbar.get_text()
        if ((url.startswith("http://") == False) and (url.startswith("https://") == False) and (url.startswith("plenja://") == False) and (url.startswith("file://") == False)):
            url = "http://" + url
        currentpage = self.notebook1.get_nth_page(self.notebook1.get_current_page())
        
        if (url.startswith("plenja://")):
            if url == "plenja://appstore":
                currentpage.load_uri(str(self.browsersettings.get_property("appstorepath")))
            else:
                with open(str(self.browsersettings.get_property("internalpath")) + url[9:]) as file1:
                    chars = file1.read()
                    currentpage.load_html(chars)
                    file1.close()
        else:
            if validators.url(url):
                currentpage.load_uri(url)
            else:
                currentpage.load_uri("https://www.google.com/search?q=" + url[7:])

        currentpage.grab_focus()
        
    def urlupdate(self, widget, arg1):
        if self.notebook1.get_nth_page(self.notebook1.get_current_page()) == widget:
            self.urlbar.set_text(widget.get_uri())
    def back(self, widget):
        currentpage = self.notebook1.get_nth_page(self.notebook1.get_current_page())
        currentpage.go_back()
        return
    def forward(self, widget):
        currentpage = self.notebook1.get_nth_page(self.notebook1.get_current_page())
        currentpage.go_forward()
        return
    def refresh(self, widget):
        currentpage = self.notebook1.get_nth_page(self.notebook1.get_current_page())
        currentpage.reload()
        return
    def notebook_create_window (self, notebook, widget, x, y):
        # handler for dropping outside of current window
        window = Gtk.Window()
        new_notebook = Gtk.Notebook()
        window.add(new_notebook)
        new_notebook.set_group_name('0') # very important for DND
        new_notebook.connect('page-removed', self.notebook_page_removed, window)
        window.connect('destroy', self.sub_window_destroyed, new_notebook, notebook)
        window.set_transient_for(self)
        window.set_destroy_with_parent(True)
        window.set_size_request(720, 480)
        window.move(x, y)
        window.show_all()
        return new_notebook

    def on_key_press(self, widget, event):
        if ((event.keyval == Gdk.KEY_Return) and (event.state & Gdk.ModifierType.CONTROL_MASK)):
            print("key tab press")
            self.new_tab(self)#, page=self.notebook1.get_n_pages() - 1)
            #self.notebook1.grab_focus()
        if ((event.keyval == Gdk.KEY_Return) and self.urlbar.has_focus()):
            print("url enter")
            self.notebook1.grab_focus()
            self.go(widget)

    def notebook_page_removed (self, notebook, child, page, window):
        #destroy the sub window after the notebook is empty
        if notebook.get_n_pages() == 0:
            window.destroy()

    def sub_window_destroyed (self, window, cur_notebook, dest_notebook):
        # if the sub window gets destroyed, push pages back to the main window
        # detach the notebook pages in reverse sequence to avoid index errors
        for page_num in reversed(range(cur_notebook.get_n_pages())):
            widget = cur_notebook.get_nth_page(page_num)
            tab_label = cur_notebook.get_tab_label(widget)
            cur_notebook.detach_tab(widget)
            dest_notebook.append_page(widget, tab_label)
            dest_notebook.set_tab_detachable(widget, True)
    def button_press(self, widget, event):
        if event.type == Gdk.EventType.BUTTON_PRESS and event.button == 1:
            #make widget popup
            widget.popup(None, None, None, event.button, event.time, event.time)
            pass
    def checkifnewtab(self, widget, page, page_number):
        print("page_number: " + str(page_number))
        print("number of pages: " + str(self.notebook1.get_n_pages()))
        if self.notebook1.get_n_pages() == page_number + 1:
            print("new tab button pressed...")
            self.new_tab(self, page_number)
    def title_changed(self, frame, title):
        #self.notebook1.set_tab_label_text(frame, frame.get_title())
        #https://eurion.net/python-snippets/snippet/Notebook%20close%20button.html
        tabbox = Gtk.Box(spacing=6)
        close_icon = Gtk.Image.new_from_icon_name("window-close-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
        
        closebutton = Gtk.Button()
        closebutton.set_relief(Gtk.ReliefStyle.NONE)
        closebutton.add(close_icon)

        closebutton.connect("clicked", self.close_tab, frame)

        tabbox.pack_start(Gtk.Label(label=frame.get_title()), True, True, 0)
        tabbox.pack_start(closebutton, True, True, 4)

        tabbox.show_all()

        self.notebook1.set_tab_label(frame, tabbox)
        
        return
    def new_tab(self, widget, page=-1):
        print("new tab")
        newtab = WebKit2.WebView()
        newtab.load_uri("https://google.com")
        newtabwrap = TabWrapper()
        newtabwrap.tab1 = newtab
        newtabwrap.notebook = self.notebook1

        newtab.connect('notify::title', self.title_changed)

        #https://eurion.net/python-snippets/snippet/Notebook%20close%20button.html
        tabbox = Gtk.Box(spacing=6)
        close_icon = Gtk.Image.new_from_icon_name("window-close-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
        
        closebutton = Gtk.Button()
        closebutton.set_relief(Gtk.ReliefStyle.NONE)
        closebutton.add(close_icon)

        closebutton.connect("clicked", self.close_tab, newtab)

        closebutton.set_can_focus(False)

        tabbox.pack_start(Gtk.Label(label="Tab " + str(self.notebook1.get_n_pages())), True, True, 0)
        tabbox.pack_start(closebutton, True, True, 4)

        tabbox.show_all()

        newtab.connect("load-changed", self.urlupdate)
        
        #self.notebook1.insert_page(newtab, Gtk.Label(label="Tab " + str(self.notebook1.get_n_pages())), self.notebook1.get_n_pages() - 1)
        #self.notebook1.insert_page(newtab, tabbox, self.notebook1.get_n_pages() - 1)
        newpagenumber = self.notebook1.get_current_page() + 1
        self.notebook1.insert_page(newtab, tabbox, newpagenumber)
        
        #self.notebook1.set_tab_detachable(newtab, True)
        self.notebook1.set_tab_reorderable(newtab, True)
        
        self.notebook1.show_all()

        self.notebook1.grab_focus()

        settings = newtab.get_settings()
        
        newtab.set_settings(settings)
        
        #if page != -1:
        #    self.notebook1.set_current_page(page)
        self.notebook1.set_current_page(newpagenumber)
    def close_tab(self, discard1, tab):
        if self.notebook1.get_n_pages() == 1:
            self.close()
        self.notebook1.prev_page()
        self.notebook1.remove(tab)

win = MainWindow()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()
