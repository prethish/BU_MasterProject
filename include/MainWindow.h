//----------------------------------------------------------------------------------------------------------------------
/// @file MainWindow.h
/// @brief a basic Qt mainQwindow used to scan the directory
/// to select the directory and load the selected file
/// @author Prethish Bhasuran
/// @brief MainWindow Class
//----------------------------------------------------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QFileDialog>
#include "GLWindow.h"

namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT // must include this if you use Qt signals/slots
public:

  explicit MainWindow(QWidget *parent = 0);

  ~MainWindow();
private slots:
  //-----------------------------------------------
  /// @brief get the selected folder using a folderDialog and
  /// populate the list with the files in the folder
  //---------------------------------------------------
  void getDirectoryPath();
  //-----------------------------------------------
  /// @brief call the GLwindow function to set the input path and object name
  //---------------------------------------------------
  void getSelectedObj();
  //-----------------------------------------------
  /// @brief turn timer ON/OFF
  //---------------------------------------------------
  void toggleTimer(bool _toggle);


private:
  Ui::MainWindow *m_ui;
  //---------------------------------------------------
  /// @brief our openGL widget
   //---------------------------------------------------
  GLWindow *m_gl;
  //---------------------------------------------------
  /// @brief selected directory Path
   //---------------------------------------------------
  QString m_dirPath;


};

#endif // MAINWINDOW_H
