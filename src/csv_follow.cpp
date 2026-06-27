
#include <algorithm>

#include <FL/Fl.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/Fl_Window.H>

#include "Fl_Csv_View.H"
#include "csv_follow_icon.h"

int main(int argc, char **argv)
{
  const int margin = 10;

  // Cree le widget hors fenetre pour connaitre sa taille naturelle
  Fl_Csv_View *csv = new Fl_Csv_View(margin, margin, 800, 400);
  csv->separator(';');
  csv->has_header(false);
  csv->value(//"nom;age;ville;profession\n"
             "Alice;30;Paris;Ingenieure\n"
             "Bob;25;Lyon;Designer\n"
             "Claire;42;Marseille;Medecin\n"
             "David;36;Toulouse;\"Chef, cuisinier\"\n");

  // Taille souhaitee = taille naturelle du contenu + marges
  int content_w, content_h;
  csv->preferred_size(content_w, content_h);
  int want_w = content_w + 2 * margin;
  int want_h = content_h + 2 * margin;

  // Recupere la zone de travail de l'ecran (hors barres systeme)
  int sx, sy, sw, sh;
  Fl::screen_work_area(sx, sy, sw, sh);

  // Borne la taille : au moins 1px (jamais zero), au plus la taille de l'ecran
  int win_w = std::max(1, std::min(want_w, sw));
  int win_h = std::max(1, std::min(want_h, sh));

  // Redimensionne le widget pour qu'il occupe exactement la fenetre finale
  csv->resize(margin, margin, win_w - 2 * margin, win_h - 2 * margin);

  Fl_Window win(win_w, win_h, "csv_follow");
  Fl_SVG_Image *app_icon = new Fl_SVG_Image(nullptr, csv_follow_svg_data);
  win.icon(app_icon);
  win.add(csv);
  win.resizable(csv);
  win.end();
  win.show(argc, argv);
  return Fl::run();
}
