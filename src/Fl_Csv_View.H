// Fl_Csv_View.H
// Widget FLTK affichant une chaine de caracteres au format CSV sous forme de tableau.
// Base sur Fl_Table_Row pour beneficier du scroll, du redimensionnement de colonnes,
// et de la selection de lignes.
//
// Usage minimal :
//
//   Fl_Csv_View *csv = new Fl_Csv_View(10, 10, 780, 580);
//   csv->separator(';');           // optionnel, ',' par defaut
//   csv->has_header(true);         // optionnel, true par defaut
//   csv->value("nom;age;ville\nAlice;30;Paris\nBob;25;Lyon\n");
//
#ifndef FL_CSV_VIEW_H
#define FL_CSV_VIEW_H

#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include <sstream>
#include <string>
#include <vector>

class Fl_Csv_View : public Fl_Table_Row
{
public:
  Fl_Csv_View(int X, int Y, int W, int H, const char *L = nullptr) : Fl_Table_Row(X, Y, W, H, L), sep_(','), quote_('"'), has_header_(true)
  {
    col_header(1);
    col_header_height(24);
    row_header(0);
    row_height_all(22);
    type(Fl_Table_Row::SELECT_SINGLE);
    end();
  }

  // Caractere separateur de champs (',' ';' '\t' ...)
  void separator(char c)
  {
    sep_ = c;
  }
  char separator() const
  {
    return sep_;
  }

  // Si true, la premiere ligne du CSV est utilisee comme en-tete de colonnes
  void has_header(bool b)
  {
    has_header_ = b;
  }
  bool has_header() const
  {
    return has_header_;
  }

  // Charge et parse une chaine CSV complete
  void value(const std::string &csv_text)
  {
    rows_.clear();
    headers_.clear();

    std::vector<std::vector<std::string>> all_rows = parse_csv(csv_text);

    if (!all_rows.empty() && has_header_)
    {
      headers_ = all_rows.front();
      all_rows.erase(all_rows.begin());
    }
    rows_ = all_rows;

    size_t ncols = headers_.size();
    for (auto &r : rows_)
      if (r.size() > ncols)
        ncols = r.size();
    ncols_ = (int)ncols;

    rows(rows_.empty() ? 1 : (int)rows_.size());
    cols(ncols_ > 0 ? ncols_ : 1);

    col_header(has_header_ ? 1 : 0);

    recompute_col_widths();
    redraw();
  }

  // Recupere le contenu d'une cellule (lecture seule)
  std::string cell(int r, int c) const
  {
    if (r < 0 || r >= (int)rows_.size())
      return "";
    if (c < 0 || c >= (int)rows_[r].size())
      return "";
    return rows_[r][c];
  }

  // Calcule la taille "naturelle" du widget pour afficher tout le contenu
  // sans scrollbars (somme des largeurs de colonnes, somme des hauteurs de
  // lignes + en-tete). Une marge est ajoutee pour la bordure du tableau.
  void preferred_size(int &w, int &h)
  {
    int total_w = 0;
    for (int c = 0; c < ncols_; ++c)
      total_w += col_widths_.size() > (size_t)c ? col_widths_[c] : 80;

    int nrows = rows_.empty() ? 1 : (int)rows_.size();
    int total_h = nrows * row_height(0);
    if (has_header_)
      total_h += col_header_height();

    // +2 pour la bordure du widget (Fl_Table dessine un cadre)
    w = total_w + 2;
    h = total_h + 2;
  }

protected:
  void draw_cell(TableContext context, int R = 0, int C = 0, int X = 0, int Y = 0, int W = 0, int H = 0) override
  {
    switch (context)
    {
    case CONTEXT_STARTPAGE:
      fl_font(FL_HELVETICA, 13);
      return;

    case CONTEXT_COL_HEADER: {
      fl_push_clip(X, Y, W, H);
      fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND_COLOR);
      std::string txt = (C < (int)headers_.size()) ? headers_[C] : ("Col " + std::to_string(C + 1));
      fl_color(FL_BLACK);
      fl_font(FL_HELVETICA_BOLD, 13);
      fl_draw(txt.c_str(), X + 4, Y, W - 8, H, FL_ALIGN_LEFT | FL_ALIGN_CLIP);
      fl_pop_clip();
      return;
    }

    case CONTEXT_CELL: {
      fl_push_clip(X, Y, W, H);
      Fl_Color bg = row_selected(R) ? FL_SELECTION_COLOR : FL_WHITE;
      fl_color(bg);
      fl_rectf(X, Y, W, H);
      fl_color(FL_LIGHT2);
      fl_rect(X, Y, W, H);

      std::string txt = cell(R, C);
      fl_color(row_selected(R) ? FL_WHITE : FL_BLACK);
      fl_font(FL_HELVETICA, 13);
      fl_draw(txt.c_str(), X + 4, Y, W - 8, H, FL_ALIGN_LEFT | FL_ALIGN_CLIP);
      fl_pop_clip();
      return;
    }

    default:
      return;
    }
  }

private:
  char sep_;
  char quote_;
  bool has_header_;
  int ncols_ = 0;
  std::vector<std::string> headers_;
  std::vector<std::vector<std::string>> rows_;
  std::vector<int> col_widths_;

  // Parseur CSV minimal gerant les guillemets et le caractere d'echappement ""
  std::vector<std::vector<std::string>> parse_csv(const std::string &text) const
  {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> row;
    std::string field;
    bool in_quotes = false;
    size_t i = 0, n = text.size();

    auto push_field = [&]() {
      row.push_back(field);
      field.clear();
    };
    auto push_row = [&]() {
      push_field();
      result.push_back(row);
      row.clear();
    };

    while (i < n)
    {
      char c = text[i];

      if (in_quotes)
      {
        if (c == quote_)
        {
          if (i + 1 < n && text[i + 1] == quote_)
          {
            field += quote_;
            i += 2;
            continue;
          }
          in_quotes = false;
          i++;
          continue;
        }
        field += c;
        i++;
        continue;
      }

      if (c == quote_)
      {
        in_quotes = true;
        i++;
        continue;
      }
      if (c == sep_)
      {
        push_field();
        i++;
        continue;
      }
      if (c == '\r')
      {
        i++;
        continue;
      }
      if (c == '\n')
      {
        push_row();
        i++;
        continue;
      }

      field += c;
      i++;
    }
    // derniere ligne si le texte ne finit pas par '\n'
    if (!field.empty() || !row.empty())
      push_row();

    // retire une eventuelle ligne vide finale
    if (!result.empty() && result.back().size() == 1 && result.back()[0].empty())
      result.pop_back();

    return result;
  }

  void recompute_col_widths()
  {
    col_widths_.assign(ncols_, 80);
    for (int c = 0; c < ncols_; ++c)
    {
      int w = 80;
      std::string h = (c < (int)headers_.size()) ? headers_[c] : "";
      w = std::max(w, (int)h.size() * 9 + 16);
      for (auto &r : rows_)
        if (c < (int)r.size())
          w = std::max(w, (int)r[c].size() * 8 + 16);
      w = std::min(w, 400);
      col_widths_[c] = w;
      col_width(c, w);
    }
  }
};

#endif // FL_CSV_VIEW_H
