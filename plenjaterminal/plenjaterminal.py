#!/usr/bin/python3
#https://python-gtk-3-tutorial.readthedocs.io/en/latest/introduction.html
#tab drag + drop is a modified version of: https://gist.github.com/ma8ma/9d5fe2e830c722dac87affe7ff3817cb
import gi
import os

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk, Vte
from gi.repository import GLib
from gi.repository import Gdk

class TerminalWrapper:
    def paste(self, arg1):
        self.terminal1.paste_clipboard()
    def copy(self, arg1):
        self.terminal1.copy_clipboard()
    def closetab(self, arg1, arg2 = None):
        self.notebook.remove(self.terminal1)
        if self.notebook.get_nth_page(0) == None:
            self.notebook.parentwindow.new_terminal(self.notebook.parentwindow)

class NotebookWrapper:
    def button_press(self, widget, event):
        if event.type == Gdk.EventType.BUTTON_PRESS and event.button == 3 and self.notebook.get_nth_page(0) == None:           #make widget popup
            widget.popup(None, None, None, event.button, event.time, event.time)
            pass

class MainWindow(Gtk.Window):
    notebook1 = Gtk.Notebook()
    def __init__(self):
        super().__init__(title="Terminal")
        self.set_default_size(720, 480)

        #self.grid1 = Gtk.Grid()
        #self.grid1.set_column_homogeneous(True)
        #self.grid1.set_column_spacing(8)

        copybutton = Gtk.Button(label="Copy")
        copybutton.connect("clicked", self.copy)
        
        pastebutton = Gtk.Button(label="Paste")
        pastebutton.connect("clicked", self.paste)

        self.terminal = []
        
        #self.terminal[0] = Vte.Terminal()
        #self.terminal.append(Vte.Terminal())
        #self.terminal[0].spawn_sync(
            #Vte.PtyFlags.DEFAULT,
            #os.environ['HOME'],
            #["/bin/sh", "--login"],
            #[],
            #GLib.SpawnFlags.DO_NOT_REAP_CHILD,
            #None,
            #None,
            #)

        #self.grid1.attach(self.terminal, 1, 2, 5, 1)
        #self.grid1.attach(copybutton, 1, 1, 1, 1)
        #self.grid1.attach(pastebutton, 2, 1, 1, 1)

        #self.terminal[0].is_focus = 1

        #self.terminal[0].connect_object("button-press-event", self.button_press, self.rcmenu)
        
        #self.add(self.grid1)
        #self.add(self.terminal)
        self.notebook1 = Gtk.Notebook()
        self.add(self.notebook1)

        self.notebook1.set_hexpand(True)
        self.notebook1.set_vexpand(True)

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

        #self.notebook1.drag_source_set(Gdk.ModifierType.BUTTON1_MASK,
        #                  [Gtk.TargetEntry.new('GTK_NOTEBOOK_TAB',
        #                                      Gtk.TargetFlags.SAME_APP, 0),
        #                  Gtk.TargetEntry.new('GTK_NOTEBOOK_TAB',
        #                                      Gtk.TargetFlags.OTHER_APP, 0)],
        #                   Gdk.DragAction.MOVE)
        
        #self.newtabbutton = Gtk.Button(label="+")
        #self.newtabbutton.connect("activate", self.new_terminal)
        #self.notebook1.append_page(self.newtabbutton)
        #self.notebook1.append_page(self.terminal[0])

        self.new_terminal(self)

        #self.add(self.notebook1)

        self.show_all()
    
    drag_page_number = 0
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
    def copy(self, widget):
        #self.terminal[0].copy_clipboard()
        #self.terminal[0].grab_focus()
        widget.copy_clipboard()
        widget.grab_focus()
    def paste(self, widget):
        #self.terminal[0].paste_clipboard()
        #self.terminal[0].grab_focus()
        widget.paste_clipboard()
        widget.grab_focus()
    def button_press(self, widget, event):
        if event.type == Gdk.EventType.BUTTON_PRESS and event.button == 3:
            #make widget popup
            widget.popup(None, None, None, event.button, event.time, event.time)
            pass
    def new_terminal(self, widget, oldtermpid = None):
        #self.terminal.append(Vte.Terminal())
        #self.terminal[self.terminalnumber] = Vte.Terminal()
        if (oldtermpid != None):
            newterm = Vte.Terminal()
            newpid = newterm.spawn_sync(
                Vte.PtyFlags.DEFAULT,
                os.readlink('/proc/{0}/cwd'.format(oldtermpid[1])),
                ["/bin/bash", "--login"],
                [],
                GLib.SpawnFlags.DO_NOT_REAP_CHILD,
                None,
                None,
                )
        else:
            newterm = Vte.Terminal()
            newpid = newterm.spawn_sync(
                Vte.PtyFlags.DEFAULT,
                os.environ['HOME'],
                ["/bin/bash", "--login"],
                [],
                GLib.SpawnFlags.DO_NOT_REAP_CHILD,
                None,
                None,
                )
        newterm.is_focus = 1
        newtermwrap = TerminalWrapper()
        newtermwrap.terminal1 = newterm
        newtermwrap.notebook = self.notebook1

        # this is the right click menu
        #https://stackoverflow.com/questions/6616270/right-click-menu-context-menu-using-pygtk
        
        rcmenu = Gtk.Menu()
        menu_item = Gtk.MenuItem("Copy")
        rcmenu.append(menu_item)
        menu_item.show()
        menu_item.connect("activate", newtermwrap.copy)
            
        menu_item = Gtk.MenuItem("Paste")
        rcmenu.append(menu_item)
        menu_item.show()
        menu_item.connect("activate", newtermwrap.paste)

        menu_item = Gtk.MenuItem("New Tab")
        rcmenu.append(menu_item)
        menu_item.show()
        menu_item.connect("activate", self.new_terminal, newpid)

        menu_item = Gtk.MenuItem("Close Tab")
        rcmenu.append(menu_item)
        menu_item.show()
        menu_item.connect("activate", newtermwrap.closetab)

        #newterm.connect_object("button-press-event", self.button_press, self.rcmenu)
        newtermwrap.labelbox = Gtk.HBox();
        newtermwrap.labelbox.add(Gtk.Label(label="Terminal " + str(self.notebook1.get_n_pages() + 1)))
        
        close_icon = Gtk.Image.new_from_icon_name("window-close-symbolic", Gtk.IconSize.SMALL_TOOLBAR)
        
        closebutton = Gtk.Button()
        closebutton.set_relief(Gtk.ReliefStyle.NONE)
        closebutton.add(close_icon)

        closebutton.connect("clicked", newtermwrap.closetab)

        newtermwrap.labelbox.add(closebutton)

        newtermwrap.labelbox.show_all()
        
        newterm.connect_object("button-press-event", self.button_press, rcmenu)
        #self.terminal.append(newterm)
        #self.remove(self.notebook1)
        #termlabel = Gtk.Label(label="Terminal")
        #termlabel.
        pagenum = self.notebook1.append_page(newterm, newtermwrap.labelbox);#Gtk.Label(label="Terminal " + str(self.notebook1.get_n_pages() + 1)))

        if self.notebook1.get_nth_page(1) == None:
            self.notebook1.set_show_tabs(False)
        else:
            self.notebook1.set_show_tabs(True)
        
        #self.notebook1.set_tab_detachable(newterm, True)
        self.notebook1.set_tab_reorderable(newterm, True)
        
        #newterm.show_all()
        #newterm.do_child_exited = print()
        newterm.connect("child-exited", newtermwrap.closetab)
        self.notebook1.show_all()
        self.notebook1.set_current_page(pagenum)
        #self.notebook1 = Gtk.Notebook()
        #for item in self.terminal:
        #    self.notebook1.append_page(item)
        #self.add(self.notebook1)
    def _sig_notebook_drag_begin(self, widget, context):
        global drag_page_number
        drag_page_number = widget.get_current_page()
        print('drag-begin:', drag_page_number, widget)


    def _sig_notebook_drag_data_get(self, widget, context, selection, info, timestamp):
        #print('drag-data-get:', drag_page_number, selection.get_target())
        #selection.set(selection.get_target(), 8, b'%d' % (drag_page_number,))
        return
    
    def _notebook_drag_data_get(self, widget, context, selection, info, timestamp):
        #print('drag-data-get:', drag_page_number, selection.get_target())
        #selection.set(selection.get_target(), 8, b'%d' % (drag_page_number,))
        return

    def _sig_drag_data_received(self, widget, context, x, y, selection, info, timestamp):
        print('drag-data-received:', selection.get_data())
        src_widget = Gtk.drag_get_source_widget(context)
        #the_page_number = int(selection.get_data())
        self.notebook1.append_page(src_widget.get_nth_page(src_widget.get_current_page()))
        #child_widget = src_widget.get_nth_page(the_page_number) 
        #child_widget.set_text('Thank you!')


    def _drag_data_received(self, widget, context, x, y, selection, info, timestamp):
        print('drag-data-received:', selection.get_data())
        src_widget = Gtk.drag_get_source_widget(context)
        the_page_number = int(selection.get_data())
        #the_page_number = src_widget.get_current_page()
        #self.notebook1.append_page(src_widget.get_nth_page(src_widget.get_current_page()))
        #child_widget = src_widget.get_nth_page(the_page_number)
        #self.drag_dest_find_target(context, None).append_page(src_widget.get_nth_page(src_widget.get_current_page()))
        #child_widget.set_text('Thank you!')
        
    def _drag_data_get(self, widget, context, selection, info, timestamp):
        print('drag-data-get:', self.drag_page_number, selection.get_target())
        selection.set(selection.get_target(), 8, b'%d' % (self.drag_page_number,))
        return


win = MainWindow()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()
