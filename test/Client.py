#!/usr/bin/python
import socket
import matplotlib
import sys
import math

import matplotlib.pylab as plt
import numpy as np
from PyQt4 import QtCore, QtGui
from test import Ui_MainWindow
from time import time

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s
try:
    from PyQt4.QtCore import QString
except ImportError:
    QString = str

class Client(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        self.ui.StartUi(self)
        self.shift = 0
        self.HOST = socket.gethostname()
        self.PORT = 0x1234
        self.MSGSIZE = 16384
        self.ifStart = False
        self.startime = 0
        self.timepassed = 0
        self.original=0;
        QtCore.QObject.connect(self.ui.originalBtn, QtCore.SIGNAL(_fromUtf8("clicked()")), self.Initial)
        QtCore.QObject.connect(self.ui.shiftUpBtn, QtCore.SIGNAL(_fromUtf8("clicked()")), self.Shiftup)
        QtCore.QObject.connect(self.ui.shiftDownBtn, QtCore.SIGNAL(_fromUtf8("clicked()")), self.Shiftdown)
        QtCore.QObject.connect(self.ui.Exit, QtCore.SIGNAL(_fromUtf8("clicked()")), self.close_application)
        QtCore.QObject.connect(self.ui.Clear, QtCore.SIGNAL(_fromUtf8("clicked()")), self.clear)
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((self.HOST, self.PORT))
        self.sendMsg=""
        
    def clear(self):
        self.ui.riskbymaturitybucket.clearContents()
        self.ui.dailychangebyissuer.clearContents()
        self.ui.timeDisplay.clear()
        self.ui.setClearHeader(1)
        self.sendMsg=""
        self.startime = time()
        
	    
    def calculate(self, Spread=(0, 0, 0, 0)):
        self.startime = time()
        self.sendMsg= "%.3f, %.3f, %.3f, %.3f, %.3f" % (self.shift, Spread[0],Spread[1], Spread[2], Spread[3])
        #print(self.sendMsg)
        self.s.send(self.sendMsg.encode('utf-8'))
        recData = repr(self.s.recv(self.MSGSIZE))
        #s.close()
        print('Received response from the server of:', repr(recData))
        self.Repopulate(recData, Spread)

    def Initial(self):
        self.shift = 0
        self.original=1;
        Spread = []
        for i in range(4):
            item = self.ui.riskbymaturitybucket.item(8, i * 2 + 2)
            print(float(item.text()))
            Spread.append(float(item.text()))
        self.calculate(Spread)

    def Shiftup(self):
        self.shift += 0.5
        self.original=0;
        Spread = []
        for i in range(4):
            item = self.ui.riskbymaturitybucket.item(8, i * 2 + 2)
            Spread.append(float(item.text()))
        self.calculate(Spread)

    def Shiftdown(self):
        self.shift -= 0.5
        self.original=0; 
        Spread = []
        for i in range(4):
            item = self.ui.riskbymaturitybucket.item(8, i * 2 + 2)
            Spread.append(float(item.text()))
        self.calculate(Spread)

    def Repopulate(self, data, Spread):
        guiData = data.split(';')
        self.ui.riskbymaturitybucket.clearContents()
        if self.original==1:
          self.ui.dailychangebyissuer.clearContents()
        self.ui.setClearHeader(self.original,Spread)
        # update Risk By Maturity
        tempData = guiData[1].split(',')
        for j in range(1, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.riskbymaturitybucket.setItem(2, j, newItem)
        tempData = guiData[2].split(',')
        for j in range(1, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.riskbymaturitybucket.setItem(3, j * 2, newItem)
        if not self.shift == 0:
            tempData = guiData[3].split(',')
            for j in range(1, len(tempData)):
                    newItem = QtGui.QTableWidgetItem(tempData[j])
                    self.ui.riskbymaturitybucket.setItem(4, j, newItem)
            tempData = guiData[4].split(',')
            for j in range(1, len(tempData)):
                    newItem = QtGui.QTableWidgetItem(tempData[j])
                    self.ui.riskbymaturitybucket.setItem(5, j * 2, newItem)
        tempData = guiData[5].split(',')
        for j in range(1, len(tempData)):
            newItem = QtGui.QTableWidgetItem(tempData[j])
            self.ui.riskbymaturitybucket.setItem(7, j * 2, newItem)
        self.ui.riskbymaturitybucket.resizeColumnsToContents()
        if self.original==1:
        # update Change By Issuer Table
            Index = 0
            rowFactor = 0
            for i in range(6, len(guiData)-4):
                tempData = guiData[i].split(',')
                if tempData[0] == 'VaR - credit':
                    Index = i
                    row = self.ui.dailychangebyissuer.rowCount()
                    self.ui.dailychangebyissuer.setRowCount(row + 1)
                    rowFactor = 3
                    break
                elif tempData[0] == 'AAA':
                    row = self.ui.dailychangebyissuer.rowCount()
                    self.ui.dailychangebyissuer.setRowCount(row + 1)
                    rowFactor = 1                
            
                row = self.ui.dailychangebyissuer.rowCount()
                self.ui.dailychangebyissuer.setRowCount(row + 1)
                for j in range(0, len(tempData)):
                    newItem = QtGui.QTableWidgetItem(tempData[j])
                    #print('data in previous for')
                    #print(tempData[j])
                    self.ui.dailychangebyissuer.setItem(i - 4 + rowFactor, j, newItem)
        
           #update VaR
            Index=21
            for k in range(3):
                tempData = guiData[Index].split(',')
                #print('tempData')
                #print(tempData)
                row = self.ui.dailychangebyissuer.rowCount()
                #print('row is')
                #print(row)
                self.ui.dailychangebyissuer.setRowCount(row + 1)
                for j in range(len(tempData)):
                    newItem = QtGui.QTableWidgetItem(tempData[j])
                    #print('tempData inside for')
                    #print(tempData[j])
                    #print('rowFactor')
                    #print(rowFactor)
                    self.ui.dailychangebyissuer.setItem(Index - 4 + rowFactor, j*3, newItem)
                Index += 1
        self.ui.dailychangebyissuer.resizeColumnsToContents()

        # update PnL & Pie and line chart
        self.timepassed = time() - self.startime
        #print('final line')
        temp=[w.replace("'", '') for w in guiData[24].split(',')]
        self.drawBarChart(temp[:-1])
        temp=guiData[21].split(',')
		
        temp2=guiData[22].split(',')
        pielist=[]          
        #pielist.append(temp[1])
        pielist.append(temp[2])
        #pielist.append(temp2[1])
        pielist.append(temp2[2])
        self.pie(pielist)
        temp=guiData[3].split(',')
        temp2=guiData[4].split(',')
        mktval=temp[2::2]
        risk=temp[1::2]
        self.linechart(risk)
        temp = guiData[0].split(',')
        clientTime = math.fabs(self.timepassed - float(temp[1]))
        Executiontime  = 'Client time: \t%.3f\n' % (clientTime)
        Executiontime +='Server time: \tReal\tUser\tSystem\tUser+Sys\n\t\t%s\t''%s\t%s\t%s' % (temp[1], temp[2], temp[3],temp[4])
        ExecutiontimetextStr = QString(Executiontime)
        self.ui.timeDisplay.setPlainText(ExecutiontimetextStr)
    
    def close_application(self):
        choice = QtGui.QMessageBox.question(self, 'Close Connection',"Do you want to close connection",QtGui.QMessageBox.Yes | QtGui.QMessageBox.No)
        if choice == QtGui.QMessageBox.Yes:
            print("Closing!!!!")
            self.s.close()
            sys.exit()
        else:
            pass		
	
    def drawBarChart(self, data):
        bardata = [ (float(x)/100) for x in data ]
        #bardata = [ (float(x)) for x in data ]
        bardata.sort()
        print(bardata)
        print('min is')
        print(min(bardata))
        print('max is')
        print(max(bardata))
        binwidth=15
        #plt.hist(bardata,bins=np.arange(min(bardata), max(bardata) + binwidth, binwidth))
        plt.hist(bardata,bins=20)
        plt.title("PnL Histogram")
        plt.xlabel("Amount of daily Pnl Changes in % change")
        plt.ylabel("Count")
        plt.show()
        
        
    def pie(self, sizes):
        labels = ['Var_Credit(Closing)','Var_Interest(Closing)']
        #print(sizes)
        abc = [ float(x)*-1 for x in sizes ]
        colors=['lightsage','crimson']
        #patches, texts = plt.pie(abc, colors=colors, startangle=90)
        #plt.legend(patches, labels, loc="best")
        plt.axis('equal')
        plt.tight_layout()
        plt.title("VaR Distribution for Closing Book")
        explode=(0, 0.1)
        pie = plt.pie(abc,colors=colors, labels=labels, explode=explode, shadow=True, autopct='%1.3f%%')
        plt.legend(pie[0], labels, loc="best")
        plt.show()
    def linechart(self,risk):
        print('inside')
        s = [ float(x) for x in risk ]
        x =[2,5,10,30]
        plt.xlim(0,35)
        my_xticks = ['T2(0-2)','T5(2-5)','T10(5-10)','T30(>10)']
        plt.xticks(x, my_xticks)
        plt.plot(x, s, marker='o', linestyle='--', color='r', label='Risk')
        plt.xlabel('Maturity Buckets')
        plt.ylabel('Risk By Maturity Buckets')
        plt.title('Risk Across Different Buckets for Closing Book')
        plt.legend()
        plt.show()

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    client = Client()
    client.show()
    sys.exit(app.exec_())