# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2011 Canonical
# Author: Alex Launi
#
# This program is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License version 3, as published 
# by the Free Software Foundation.
#
# This script is designed to run unity in a test drive manner. It will drive 
# X and test the GL calls that Unity makes, so that we can easily find out if
# we are triggering graphics driver/X bugs.

"""
A collection of emulators that make it easier to interact with Unity.
"""

from compizconfig import Setting
from compizconfig import Plugin
import dbus
from time import sleep

from autopilot.emulators.X11 import Keyboard, Mouse
from autopilot.globals import global_context

class Unity(object):
    '''
    High level class to abstract interactions with the unity shell.

    This class should not be used directly. Instead, use one of the derived classes to
    interact with a different piece of the Unity system.
    '''

    def __init__(self):
        ## TODO
        # check that unity is running
        # acquire the debugging dbus object
        self.UNITY_BUS_NAME = 'com.canonical.Unity'
        self.DEBUG_PATH = '/com/canonical/Unity/Debug'
        self.INTROSPECTION_IFACE = 'com.canonical.Unity.Debug.Introspection'
        self._mouse = Mouse()
        self._keyboard = Keyboard()

        self._bus = dbus.SessionBus()
        self._debug_proxy_obj = self._bus.get_object(self.UNITY_BUS_NAME,
                                                     self.DEBUG_PATH)
        self._introspection_iface = dbus.Interface(self._debug_proxy_obj,
                                                   self.INTROSPECTION_IFACE)

    def get_state(self, piece='/Unity'):
        '''
        returns a full dump of unity's state via the introspection interface
        '''
        return self._introspection_iface.GetState(piece)


class Launcher(Unity):
    """
    Interact with the unity Launcher.
    """

    def __init__(self):
        super(Launcher, self).__init__()
        # set up base launcher vars
        state = self.__get_state()
        self.icon_width = int(state['icon-size'])

        self.reveal_pos = (0, 120)
        self.hide_pos = (self.icon_width *2, 120)
        
        self.show_timeout = 1
        self.hide_timeout = 1
        

    def move_mouse_to_reveal_pos(self):
        self._mouse.move(*self.reveal_pos)
        sleep(self.show_timeout)

    def move_mouse_outside_of_boundry(self):
        self._mouse.move(*self.hide_pos)
        sleep(self.hide_timeout)

    def is_showing(self):
        state = self.__get_state()
        return not bool(state['hidden'])
    
    def __get_state(self):
        # get the state for the 'launcher' piece
        return super(Launcher, self).get_state('/Unity/Launcher')[0]

    def get_launcher_icons(self):
        """
        Get a list of launcher icons in this launcher.
        """
        icons = self.get_state("//Launcher/LauncherIcon")
        return [LauncherIcon(icon_dict) for icon_dict in icons]

    def click_launcher_icon(self, icon, button=1):
        """
        Move the mouse over the launcher icon, and click it.
        """
        self.move_mouse_to_reveal_pos()
        self._mouse.move(icon.x, icon.y + (self.icon_width / 2))
        self._mouse.click(button)
        self.move_mouse_outside_of_boundry()


class LauncherIcon:
    """
    Holds information about a launcher icon. Do not instantiate an instance
    of this class yourself. Instead, use the appropriate methods in the Launcher
    class instead.
    """

    def __init__(self, icon_dict):
        self.tooltip_text = icon_dict['tooltip-text']
        self.x = icon_dict['x']
        self.y = icon_dict['y']
        self.num_windows = icon_dict['related-windows']
        self.visible = icon_dict['quirk-visible']
        self.active = icon_dict['quirk-active']
        self.running = icon_dict['quirk-running']
        self.presented = icon_dict['quirk-presented']
        self.urgent = icon_dict['quirk-urgent']

class Switcher(Unity):
    """
    Interact with the Unity switcher.
    """

    def __init__(self):
        super(Switcher, self).__init__()

    def initiate(self):
        self._keyboard.press('^A^T')
        self._keyboard.release('^T')

    def initiate_detail_mode(self):
        self._keyboard.press('^A`')
        self._keyboard.release('`')

    def terminate(self):
        self._keyboard.release('^A')

    def next_icon(self):
        self._keyboard.press_and_release('^T')

    def previous_icon(self):
        self._keyboard.press_and_release('^S^T')

    def show_details(self):
        self._keyboard.press_and_release('`')

    def hide_details(self):
        self._keyboard.press_and_release('^U')

    def next_detail(self):
        self._keyboard.press_and_release('`')

    def previous_detail(self):
        self._keyboard.press_and_release('^S`')

    def __get_icon(self, index):
        import ipdb; ipdb.set_trace()
        return self.get_state('/Unity/SwitcherController/SwitcherModel')[0]['children-of-men'][index][1][0]

    def get_icon_name(self, index):
        return self.__get_icon(index)['tooltip-text']

    def get_icon_desktop_file(self, index):
        try:
            return self.__get_icon(index)['desktop-file']
        except:
            return None

    def get_model_size(self):
        return len(self.get_state('/Unity/SwitcherController/SwitcherModel')[0]['children-of-men'])

    def get_selection_index(self):
        return int(self.get_state('/Unity/SwitcherController/SwitcherModel')[0]['selection-index'])

    def get_last_selection_index(self):
        return bool(self.get_state('/Unity/SwitcherController/SwitcherModel')[0]['last-selection-index'])

    def get_is_visible(self):
        return bool(self.get_state('/Unity/SwitcherController')[0]['visible'])

class Dash(Unity):
    """
    An emulator class that makes it easier to interact with the unity dash.
    """

    def __init__(self):
        self.plugin = Plugin(global_context, "unityshell")
        self.setting = Setting(self.plugin, "show_launcher")
        super(Dash, self).__init__()

    def toggle_reveal(self):
        """
        Reveals the dash if it's currently hidden, hides it otherwise.
        """
        self._keyboard.press_and_release("^W")
        sleep(1)

    def ensure_visible(self):
        """
        Ensures the dash is visible.
        """
        if not self.get_is_visible():
            self.toggle_reveal();

    def ensure_hidden(self):
        """
        Ensures the dash is hidden.
        """
        if self.get_is_visible():
            self.toggle_reveal();

    def get_is_visible(self):
        """
        Is the dash visible?
        """
        return bool(self.get_state("/Unity/DashController")[0]["visible"])

    def get_search_string(self):
        """
        Return the current dash search bar search string.
        """
        return unicode(self.get_state("//SearchBar")[0]['search_string'])

    def get_current_lens(self):
        """
        Returns the id of the current lens. For example, the default lens is
        'home.lens', the run-command lens is 'commands.lens'.
        """
        return unicode(self.get_state("//DashController/DashView/LensBar")[0]['active-lens'])

    def reveal_application_lens(self):
        """
        Reveal the application lense.
        """
        self._keyboard.press("^W")
        self._keyboard.press_and_release("a")
        self._keyboard.release("^W")

    def reveal_music_lens(self):
        """
        Reveal the music lense.
        """
        self._keyboard.press("^W")
        self._keyboard.press_and_release("m")
        self._keyboard.release("^W")

    def reveal_file_lens(self):
        """
        Reveal the file lense.
        """
        self._keyboard.press("^W")
        self._keyboard.press_and_release("f")
        self._keyboard.release("^W")

    def reveal_command_lens(self):
        """
        Reveal the 'run command' lens.
        """
        self._keyboard.press_and_release(['Alt_L','F2'])
