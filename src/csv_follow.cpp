// main.cpp - demo Fl_Csv_View
#include "Fl_Csv_View.H"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

int main(int argc, char **argv)
{
  Fl_Window win(820, 420, "Demo Fl_Csv_View");

  Fl_Csv_View *csv = new Fl_Csv_View(10, 10, 800, 400);
  csv->separator(';');
  csv->has_header(true);
  csv->value("nom;age;ville;profession\n"
             "Alice;30;Paris;Ingenieure\n"
             "Bob;25;Lyon;Designer\n"
             "Claire;42;Marseille;Medecin\n"
             "David;36;Toulouse;\"Chef, cuisinier\"\n");

  win.resizable(csv);
  win.end();
  win.show(argc, argv);
  return Fl::run();
}
