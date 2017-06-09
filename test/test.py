from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_MainWindow(object):
    def setClearHeader(self,flag,Spread=(0, 0, 0, 0)):
        maturitylbl = ('Book', 'Risk', 'Mkt Value', 'Risk', 'Mkt Value', 'Risk','Mkt Value', 'Risk', 'Mkt Value')
        self.riskbymaturitybucket.setColumnCount(len(maturitylbl))
        self.riskbymaturitybucket.setHorizontalHeaderLabels(maturitylbl)
        self.riskbymaturitybucket.verticalHeader().setVisible(False)
        self.riskbymaturitybucket.horizontalHeader().setVisible(False)
        self.riskbymaturitybucket.setRowCount(9)
        firstLineLabels = ('2YR', '5YR', '10YR', '30YR')
        newItem = QtGui.QTableWidgetItem('Book')
        #newItem.setBackground(QtGui.QColor(0,102,255))
        self.riskbymaturitybucket.setItem(0, 0, newItem)
        for i in range(len(firstLineLabels)):
            newItem = QtGui.QTableWidgetItem(firstLineLabels[i])
            if i==0:
                newItem.setBackground(QtGui.QColor(255,204,153))
            elif i==1:
                newItem.setBackground(QtGui.QColor(255,255,153))
            elif i==2:
                newItem.setBackground(QtGui.QColor(204,255,153))
            else:
                newItem.setBackground(QtGui.QColor(204,255,255))
            self.riskbymaturitybucket.setItem(0, 2 * i + 1, newItem)
            self.riskbymaturitybucket.setSpan(0, 2 * i + 1, 1, 2)
        for i in range(1, len(maturitylbl)):
            newItem = QtGui.QTableWidgetItem(maturitylbl[i])
            self.riskbymaturitybucket.setItem(1, i, newItem)
        itemLabels = ('Closing File','2yr Hedge:','Shifted Closing File','Shifted 2yr Hedge:')
        for i in range (len(itemLabels)):
            newItem = QtGui.QTableWidgetItem(itemLabels[i])
            self.riskbymaturitybucket.setItem(i + 2, 0, newItem)
        newItem = QtGui.QTableWidgetItem('Yield Curve')
        self.riskbymaturitybucket.setItem(7, 0, newItem)
        newItem = QtGui.QTableWidgetItem('Spread')
        self.riskbymaturitybucket.setItem(8, 0, newItem)
        for i in range(len(Spread)):
            string = "%.3f" % Spread[i]
            newItem = QtGui.QTableWidgetItem(string)
            self.riskbymaturitybucket.setItem(8, i * 2 + 2, newItem)
        self.riskbymaturitybucket.resizeColumnsToContents()
          
        if flag == 1:
           dctLabels = ('Ticker', 'Amount', 'Risk', 'LGD', 'Amount', 'Risk','LGD', 'Amount', 'Risk', 'LGD')
           self.dailychangebyissuer.setColumnCount(len(dctLabels))
           self.dailychangebyissuer.setHorizontalHeaderLabels(dctLabels)
           self.dailychangebyissuer.verticalHeader().setVisible(False)
           self.dailychangebyissuer.horizontalHeader().setVisible(False)
           self.dailychangebyissuer.setRowCount(2)
           newItem = QtGui.QTableWidgetItem('Opening Position')
           newItem.setBackground(QtGui.QColor(255,204,153))
         
           newItem.setTextAlignment(0x0004 | 0x0080)
           self.dailychangebyissuer.setItem(0, 1, newItem)
           self.dailychangebyissuer.setSpan(0, 1, 1, 3)
           newItem = QtGui.QTableWidgetItem('Closing Position')
           newItem.setBackground(QtGui.QColor(255,255,153))
           self.dailychangebyissuer.setItem(0, 4, newItem)
           self.dailychangebyissuer.setSpan(0, 4, 1, 3)
           newItem = QtGui.QTableWidgetItem('Change')
           newItem.setBackground(QtGui.QColor(204,255,153))
           self.dailychangebyissuer.setItem(0, 7, newItem)
           self.dailychangebyissuer.setSpan(0, 7, 1, 3)
           for i in range(len(dctLabels)):
               newItem = QtGui.QTableWidgetItem(dctLabels[i])
               self.dailychangebyissuer.setItem(1, i, newItem)
           self.dailychangebyissuer.resizeColumnsToContents()
		
    def download(self):
        self.completed = 0
        while self.completed < 100:
              self.completed += 0.01
              self.progress.setValue(self.completed)

    def StartUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(1800, 850)
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "BOND RISK MANAGEMENT", None, QtGui.QApplication.UnicodeUTF8))
        MainWindow.setAutoFillBackground(True)

        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))

        self.maturityBucketTableTitle = QtGui.QLabel(self.centralwidget)
        self.maturityBucketTableTitle.setGeometry(QtCore.QRect(1150, 5, 500, 40))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(60)
        self.maturityBucketTableTitle.setFont(font)
        self.maturityBucketTableTitle.setText(QtGui.QApplication.translate("MainWindow", "Risk by Maturity(amounts in 000\'s)", None, QtGui.QApplication.UnicodeUTF8))
        self.maturityBucketTableTitle.setObjectName(_fromUtf8("maturityBucketTableTitle"))

        self.riskbymaturitybucket = QtGui.QTableWidget(self.centralwidget)
        self.riskbymaturitybucket.setGeometry(QtCore.QRect(900, 55, 753, 320))
        self.riskbymaturitybucket.setObjectName(_fromUtf8("maturityBucketTable"))
        self.riskbymaturitybucket.setAutoFillBackground(True)
        self.riskbymaturitybucket.setAlternatingRowColors(True)

        self.dailychangebyissuer = QtGui.QLabel(self.centralwidget)
        self.dailychangebyissuer.setGeometry(QtCore.QRect(180, 5, 390, 40))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setPointSize(10)
        font.setBold(True)
        font.setWeight(60)
        self.dailychangebyissuer.setFont(font)
        self.dailychangebyissuer.setText(QtGui.QApplication.translate("MainWindow", "Daily Change by Issuer (amounts in 000\'s)", None, QtGui.QApplication.UnicodeUTF8))
        self.dailychangebyissuer.setObjectName(_fromUtf8("dailychangebyissuer"))

        self.dailychangebyissuer = QtGui.QTableWidget(self.centralwidget)
        self.dailychangebyissuer.setGeometry(QtCore.QRect(10, 55, 720, 750))
        self.dailychangebyissuer.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedStates))
        self.dailychangebyissuer.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.dailychangebyissuer.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.dailychangebyissuer.setObjectName(_fromUtf8("dailychangebyissuer"))
        self.dailychangebyissuer.setAutoFillBackground(True)
        self.dailychangebyissuer.setAlternatingRowColors(True)

        self.setClearHeader(1)

        self.shiftLabel = QtGui.QLabel(self.centralwidget)
        self.shiftLabel.setGeometry(QtCore.QRect(770, 420, 141, 20))
        self.shiftLabel.setText(QtGui.QApplication.translate("MainWindow", "Shifting Choices:", None, QtGui.QApplication.UnicodeUTF8))
        self.shiftLabel.setObjectName(_fromUtf8("shiftLabel"))

        self.shiftUpBtn = QtGui.QPushButton(self.centralwidget)
        self.shiftUpBtn.setGeometry(QtCore.QRect(920, 420, 130, 27))
        self.shiftUpBtn.setText(QtGui.QApplication.translate("MainWindow", "Shift 50% Up", None, QtGui.QApplication.UnicodeUTF8))
        self.shiftUpBtn.setObjectName(_fromUtf8("shiftUpBtn"))

        self.shiftDownBtn = QtGui.QPushButton(self.centralwidget)
        self.shiftDownBtn.setGeometry(QtCore.QRect(1070,420,130, 27))
        self.shiftDownBtn.setText(QtGui.QApplication.translate("MainWindow", "Shift 50% Down", None, QtGui.QApplication.UnicodeUTF8))
        self.shiftDownBtn.setObjectName(_fromUtf8("shiftDownBtn"))

        self.originalBtn = QtGui.QPushButton(self.centralwidget)
        self.originalBtn.setGeometry(QtCore.QRect(1220, 420, 130, 27))
        self.originalBtn.setText(QtGui.QApplication.translate("MainWindow", "Initial", None, QtGui.QApplication.UnicodeUTF8))
        self.originalBtn.setObjectName(_fromUtf8("originalBtn"))

        self.Exit = QtGui.QPushButton(self.centralwidget)
        self.Exit.setGeometry(QtCore.QRect(1370, 420, 130, 27))
        self.Exit.setText(QtGui.QApplication.translate("MainWindow", "Exit", None, QtGui.QApplication.UnicodeUTF8))
        self.Exit.setObjectName(_fromUtf8("Exit"))
        	
		
        self.Clear = QtGui.QPushButton(self.centralwidget)
        self.Clear.setGeometry(QtCore.QRect(1520, 420, 130, 27))
        self.Clear.setText(QtGui.QApplication.translate("MainWindow", "Clear", None, QtGui.QApplication.UnicodeUTF8))
        self.Clear.setObjectName(_fromUtf8("Clear"))
        self.Clear.clicked.connect(self.download)
		
        self.timeDisplay = QtGui.QTextEdit(self.centralwidget)
        self.timeDisplay.setGeometry(QtCore.QRect(1000, 570, 481, 91))
        self.timeDisplay.setReadOnly(True)
        self.timeDisplay.setObjectName(_fromUtf8("timeDisplay"))
        self.timeDisplay.setTextColor(QtGui.QColor(165,42,42))
        
        self.timeLabel = QtGui.QLabel(self.centralwidget)
        self.timeLabel.setGeometry(QtCore.QRect(770, 600, 141, 20))
        self.timeLabel.setText(QtGui.QApplication.translate("MainWindow", "Execution Time:", None, QtGui.QApplication.UnicodeUTF8))
        self.timeLabel.setObjectName(_fromUtf8("Execution Time"))
        
        self.progressLabel = QtGui.QLabel(self.centralwidget)
        self.progressLabel.setGeometry(QtCore.QRect(1375, 495, 141, 20))
        self.progressLabel.setText(QtGui.QApplication.translate("MainWindow", "Clear Progress:", None, QtGui.QApplication.UnicodeUTF8))
        self.progress = QtGui.QProgressBar(self.centralwidget)
        self.progress.setGeometry(QtCore.QRect(1520, 490, 180, 27))

        MainWindow.setCentralWidget(self.centralwidget)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1440, 23))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)
		
		
		
		
		
        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)
		
		
		
		
		
    def retranslateUi(self, MainWindow):
        pass

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    MainWindow = QtGui.QMainWindow()
    ui = Ui_MainWindow()
    ui.StartUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
