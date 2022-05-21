import os
import shutil
import json

class browsersettings:
    def __init__(self):
        homedir = os.path.expanduser('~')
        if homedir == '':
            raise ValueError("Couldn't find the home directory.")
        if os.path.exists(homedir + '/.plenja/browser/settings.json') == False:
            # create new profile
            try:
                os.mkdir(homedir + '/.plenja')
            except:
                print("")
            try:
                os.mkdir(homedir + '/.plenja/browser')
            except:
                print("")
            try:
                shutil.copy('/etc/plenja/browser/settings.json.default', homedir + '/.plenja/browser/settings.json')
            except:
                print("")
        settingsfile = homedir + '/.plenja/browser/settings.json'
        prefs = ''
        with open(settingsfile) as file1:
            prefs = file1.read()
            file1.close()
        self.settingsdict = json.loads(prefs)
    def set_property(self, name, value):
        self.settingsdict[name] = value
    def get_property(self, name):
        return self.settingsdict[name]
    def save(self):
        json.dumps()
        
