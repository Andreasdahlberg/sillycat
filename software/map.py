import sys
import os
from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4.QtWebKit import *
import ctypes
import time
import simplejson as json

ICON = os.path.join('../../../software/icons', 'Tunning-icon.png')
RAMSIZE = 2048

class Item(object):

    def __init__(self, name=None, section=None, size=0, address=0, object_file=None):
        self.name = name
        self.section = section
        self.size = size
        self.address = address
        self.object_file = object_file


    def to_dictionary(self):
        return {'name': self.name,
                'section': self.section,
                'size': self.size,
                'address': self.address,
                'object_file': self.object_file}


    def to_json(self):
        return json.dumps(self.to_dictionary())


    def valid(self):
        return (self.address > 0 and self.size > 0)


class bssItem(Item):

    def __init__(self):
        super().__init__()
        self.section = 'bss'


    def parse(self, lines):
        try:

            if (len(lines) == 2):
                tmp = lines[1].strip().split()
                self.name = lines[0].strip()[5:].split('.')[0]
                self.object_file = tmp[2]
                self.size = int(tmp[1], 16)
                self.address = int(tmp[0], 16)
            else:

                line = lines[0].strip()[5:]
                self.name = line.split()[0]
                self.object_file = line.split()[3]
                self.size = int(line.split()[2], 16)
                self.address = int(line.split()[1], 16)
        except:
            print('--Error:', lines)

class dataItem(Item):

    def __init__(self):
        super().__init__()
        self.section = 'rodata'


    def parse(self, lines):
        try:
            tmp = lines[1].strip().split()

            self.name = lines[0].strip()[8:].split('.')[0]
            self.object_file = tmp[2]
            self.size = int(tmp[1], 16)
            self.address = int(tmp[0], 16)
        except:
            print('***********next error************')
            print(lines)

class CodeGenerator():
    def __init(self):
        pass

    def demo(self, data):
        values = []
        labels = []
        total_size = 0


        for item in data:
            if (item.name in labels):
                idx = labels.index(item.name)
                values[idx] = values[idx] + item.size
            else:

                values.append(item.size)
                labels.append(item.name)

            print(item.to_json())

            total_size = total_size + item.size

        values.append(RAMSIZE - total_size)
        labels.append('free')
        print('Total size:', total_size)


        f = open('../../../software/template.js', encoding='utf-8')
        m = open('../../../software/nice.js', 'w')        

        for line in f.readlines():
            line = line.replace('$values$', json.dumps(values))
            line = line.replace('$labels$', json.dumps(labels))  

            m.write(line)
        f.close()
        m.close()       


class MapParser():
    def __init__(self, map_file):
        self._map_file = map_file

        self._sections = {'.bss.': bssItem, '.rodata': dataItem}
        self._items = []


    def parse(self):
        f = open(self._map_file, encoding='utf-8')
        lines = f.readlines()

        index = 0
        for line in lines:
            for key in self._sections:
                if (line.strip().startswith(key) and line.strip().endswith('.o')):
                    item = self._sections[key]()
                    item.parse([lines[index]])

                    if (item.valid()):
                        self._items.append(item)      

                elif (line.strip().startswith(key)):
                    item = self._sections[key]()
                    item.parse([lines[index], lines[index + 1]])

                    if (item.valid()):
                        self._items.append(item)           


            index = index + 1

        return self._items

class MemoryView(QWebView):
    def __init__(self):
        super(MemoryView, self).__init__()

        self.resize(750, 550)
        #self.center()
        self.setWindowTitle('MemView Tool')
        self.setWindowIcon(QIcon(ICON))       
        self.setContextMenuPolicy(Qt.ActionsContextMenu);

        #self.thread = AThread()
        #self.thread.finished.connect(self.view_result)
        #self.thread.start()

        url = 'file:///C:/Users/Andreas/Documents/GitHub/sillycat/software/nice.html'
        self.load(QUrl(url))
        self.show()
    
    def center(self):    
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())  

    def view_result(self):    
        self.load(QUrl(self.thread.url))


def main():
    myappid = 'memory.software.15' 
    ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

    app = QApplication(sys.argv)
    ex = MemoryView()
    sys.exit(app.exec_())


if __name__ == '__main__':
    if (len(sys.argv) <= 1):
        print('Missing arguments')
        map_file = '../firmware/node_firmware/Debug/node_firmware.map'
    else:
        map_file = sys.argv[1]
       

    parser = MapParser(map_file)

    items = parser.parse()

    cg = CodeGenerator()
    cg.demo(items)
    
    main()