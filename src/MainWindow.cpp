//----------------------------------------------------------------------------------------------------------------------
/// @file MainWindow.cpp
/// @author Prethish Bhasuran
/// @brief MainWindow Class member functions
//----------------------------------------------------------------------------------------------------------------------

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);
  // create an openGL format and pass to the new GLWidget
  QGLFormat format;
  format.setVersion(4, 1);
  format.setProfile(QGLFormat::CoreProfile);
  m_gl = new  GLWindow(format, this);
  m_ui->s_mainGridLayout->addWidget(m_gl, 0, 0, 2, 1);
  //load mesh
  connect(m_ui->m_loadPath, SIGNAL(clicked()), this, SLOT(getDirectoryPath()));
  connect(m_ui->m_loadObj, SIGNAL(clicked()), this, SLOT(getSelectedObj()));
  //skin algorithm
  connect(m_ui->m_skinType, SIGNAL(currentIndexChanged(int)), m_gl, SLOT(setSkinAlgorithm(int)));
//shader
  connect(m_ui->m_wireframe, SIGNAL(clicked(bool)), m_gl, SLOT(toggleWireframe(bool)));
  connect(m_ui->m_colour, SIGNAL(clicked()), m_gl, SLOT(setColour()));
  //timer
  connect(m_ui->m_toggleAnim, SIGNAL(clicked(bool)), m_gl, SLOT(toggleMainTimer(bool)));
  connect(m_ui->m_plusFrame, SIGNAL(clicked(bool)), m_gl, SLOT(incrementFrame()));
  connect(m_ui->m_minusFrame, SIGNAL(clicked(bool)), m_gl, SLOT(decrementFrame()));
  //debug
  connect(m_ui->m_debugFPS, SIGNAL(toggled(bool)), m_gl , SLOT(toggleDebugInfo(bool)));

}

MainWindow::~MainWindow()
{
  delete m_ui;
}

void MainWindow::getDirectoryPath()
{
  m_dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
              "./",
              QFileDialog::ShowDirsOnly
              | QFileDialog::DontResolveSymlinks);
  m_ui->m_directoryPath->setText(m_dirPath);

  QDir currentDir = QDir(m_dirPath);
  QStringList files;
  QString fileName;
  if (fileName.isEmpty())
    fileName = "*";
  files = currentDir.entryList(QStringList(fileName),
                               QDir::Files | QDir::NoSymLinks);
  for (int i = 0; i < files.size(); ++i) {
    m_ui->m_objectSelection->addItem(files[i]);
  }
}

void MainWindow::getSelectedObj()
{
  QString obj = m_ui->m_objectSelection->currentText();
  m_gl->loadObj(m_dirPath.toStdString(), obj.toStdString());
  m_ui->m_skinType->setCurrentIndex(0);
}

void MainWindow::toggleTimer(bool _toggle)
{
  m_gl->toggleMainTimer(_toggle);
}



