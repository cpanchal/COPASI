#include <fstream>
#include <string>
#include <vector>
#include <qmainwindow.h>
#include <qtoolbutton.h>
#include <qtimer.h>

class PlotTaskSpec;
class CopasiPlot;

class PlotWindow : public QMainWindow
  {
    Q_OBJECT

    // a timer that controls automatic updates of the plot
    QTimer* timer;

    // points to the specification of the plot inside this window - temporary perhaps
    PlotTaskSpec* ptspec;

    // points to the plot instance inside this window
    CopasiPlot *plot;

  public:
    QToolButton * zoomButton;

    PlotWindow(std::istream& targetfile);

    ~PlotWindow();

    // reloads the plot inside the window, updating it as necessary
    void reloadPlot(PlotTaskSpec* plotspec, std::vector<int> deletedCurveKeys);

  private slots:
    void append();

    // let the plot updates itself at regular intervals - the length of the interval is fixed for now
    void autoUpdate(bool toggled);

    void enableZoom();

    void mouseReleased(const QMouseEvent &e);

    // prints the plot (to printer or file)
    void printPlot();
  };
