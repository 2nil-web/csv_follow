
#include <FL/Fl.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_Window.H>

#include "Fl_Csv_View.H"
#include "csv_follow_icon.h"

int main(int argc, char **argv)
{
  Fl_Window win(820, 420, "Demo Fl_Csv_View");

  Fl_Csv_View *csv = new Fl_Csv_View(10, 10, 800, 400);
  Fl_SVG_Image *app_icon = new Fl_SVG_Image(nullptr, csv_follow_svg_data);
  win.icon(app_icon);

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
