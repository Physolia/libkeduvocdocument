/***************************************************************************
                        Vocabulary Document for KDE Edu
    -----------------------------------------------------------------------
    copyright      : (C) 1999-2001 Ewald Arnold
                     (C) 2001 The KDE-EDU team
                     (C) 2005-2007 Peter Hedlund <peter.hedlund@kdemail.net>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keduvocdocument.h"

#include <QApplication>
#include <QFileInfo>
#include <QTextStream>
#include <QtAlgorithms>
#include <QIODevice>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <krandomsequence.h>
#include <kfilterdev.h>

#include "keduvocexpression.h"
#include "keduvockvtmlwriter.h"
#include "keduvoccsvreader.h"
#include "keduvoccsvwriter.h"
#include "keduvockvtmlreader.h"
#include "keduvocwqlreader.h"
#include "keduvocpaukerreader.h"
#include "keduvocvokabelnreader.h"
#include "keduvocxdxfreader.h"
#include "leitnersystem.h"

class KEduVocDocument::Private
{
public:
  Private(KEduVocDocument* qq)
    : q(qq)
  {
    init();
  }

  void init();

  KEduVocDocument* q;

  bool                      m_dirty;
  KUrl                      m_url;
  QList<bool>               m_sortIdentifier;
  bool                      m_sortLesson;
  bool                      m_sortingEnabled;

  // save these to document
  QStringList               m_identifiers;      //0= origin, 1,.. translations
  int                       m_currentLesson;
  QList<int>                m_extraSizeHints;
  QList<int>                m_sizeHints;

  QString                   m_generator;
  QString                   m_queryorg;
  QString                   m_querytrans;
  QList<KEduVocExpression>  m_vocabulary;
  QList<int>                m_lessonsInQuery;
  QStringList               m_lessonDescriptions;
  QStringList               m_typeDescriptions;
  QStringList               m_tenseDescriptions;
  QStringList               m_usageDescriptions;
  QString                   m_title;
  QString                   m_author;
  QString                   m_license;
  QString                   m_remark;
  QString                   m_version;
  QString                   m_csvDelimiter;

  QList<KEduVocArticle>     m_articles;
  QList<KEduVocConjugation> m_conjugations;

  LeitnerSystem*            m_leitnerSystem;
  bool                      m_activeLeitnerSystem;
};


void KEduVocDocument::Private::init()
{
  m_lessonDescriptions.clear();
  m_articles.clear();
  m_typeDescriptions.clear();
  m_tenseDescriptions.clear();
  m_identifiers.clear();
  m_sortIdentifier.clear();
  m_extraSizeHints.clear();
  m_sizeHints.clear();
  m_vocabulary.clear();
  m_dirty = false;
  m_sortingEnabled = true;
  m_sortLesson = false;
  m_currentLesson = 0;
  m_queryorg = "";
  m_querytrans = "";
  m_url.setFileName(i18n("Untitled"));
  m_title = "";
  m_author = "";
  m_remark = "";
  m_version = "";
  m_generator = "";
  m_csvDelimiter = QString('\t');
  m_activeLeitnerSystem = false;
  m_leitnerSystem = NULL;
}


/**@todo possibly implement
  1. sorting based on lesson name
  2. sorting based on lesson index and original.
*/

class KEduVocDocumentSortHelper
{
public:
  inline KEduVocDocumentSortHelper(int column, Qt::SortOrder order) : sort_column(column), sort_order(order) {}

  inline bool operator()(const KEduVocExpression &e1, const KEduVocExpression &e2) const
    {
    if (sort_order == Qt::AscendingOrder)
      if (sort_column == 0)
        return e1.original().toLower() < e2.original().toLower();
      else
        return e1.translation(sort_column).toLower() < e2.translation(sort_column).toLower();
    else
      if (sort_column == 0)
        return !(e1.original().toLower() < e2.original().toLower());
      else
        return !(e1.translation(sort_column).toLower() < e2.translation(sort_column).toLower());
    }

private:
  int sort_column;
  Qt::SortOrder sort_order;
};


KEduVocDocument::KEduVocDocument(QObject *parent)
  : QObject(parent), d(new Private(this))
{
}


KEduVocDocument::~KEduVocDocument()
{
  delete d;
}


void KEduVocDocument::setModified(bool dirty)
{
  d->m_dirty = dirty;
  emit docModified(d->m_dirty);
}


void KEduVocDocument::appendEntry(KEduVocExpression *expression)
{
  d->m_vocabulary.append(*expression);
  setModified();
}


void KEduVocDocument::insertEntry(KEduVocExpression *expression, int index)
{
  d->m_vocabulary.insert(index, *expression);
  setModified();
}


KEduVocDocument::FileType KEduVocDocument::detectFileType(const QString &fileName)
{
  QIODevice * f = KFilterDev::deviceForFile(fileName);
  if (!f->open(QIODevice::ReadOnly))
    return csv;

  QDataStream is(f);

  qint8 c1, c2, c3, c4, c5;
  is >> c1
    >> c2
    >> c3
    >> c4
    >> c5;  // guess filetype by first x bytes

  QTextStream ts(f);
  QString line;
  QString line2;

  line = ts.readLine();
  if (!ts.atEnd())
    line2 = ts.readLine();
  line.prepend(c5);
  line.prepend(c4);
  line.prepend(c3);
  line.prepend(c2);
  line.prepend(c1);

  if (!is.device()->isOpen())
    return kvd_none;

  f->close();
  if (c1 == '<' && c2 == '?' && c3 == 'x' && c4 == 'm' && c5 == 'l') {
    if (line2.indexOf("pauker", 0) >  0)
      return pauker;
    else if (line2.indexOf("xdxf", 0) >  0)
      return xdxf;
    else
      return kvtml;
  }

  if (line == WQL_IDENT)
    return wql;

  if (c1 == '"' && (line.contains('"') == 1 || line.contains(QRegExp("\",[0-9]"))))
    return vokabeln;

  return csv;
}


bool KEduVocDocument::open(const KUrl& url)
{
  d->init();
  if (!url.isEmpty())
    d->m_url = url;

  bool read = false;
  QString errorMessage = i18n("<qt>Cannot open file<br><b>%1</b></qt>", url.path());
  QString temporaryFile;
  if (KIO::NetAccess::download(url, temporaryFile, 0))
  {
    QIODevice * f = KFilterDev::deviceForFile(temporaryFile);

    if (!f->open(QIODevice::ReadOnly))
    {
      KMessageBox::error(0, errorMessage);
      return false;
    }

    FileType ft = detectFileType(url.path());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    switch (ft) {
      case kvtml:
      {
        KEduVocKvtmlReader kvtmlReader(f);
        read = kvtmlReader.readDoc(this);
        if (!read)
          errorMessage = kvtmlReader.errorMessage();
      }
      break;

      case wql:
      {
        KEduVocWqlReader wqlReader(f);
        d->m_url.setFileName(i18n("Untitled"));
        read = wqlReader.readDoc(this);
        if (!read)
          errorMessage = wqlReader.errorMessage();
      }
      break;

      case pauker:
      {
        KEduVocPaukerReader paukerReader(f);
        d->m_url.setFileName(i18n("Untitled"));
        read = paukerReader.readDoc(this);
        if (!read)
          errorMessage = paukerReader.errorMessage();
      }
      break;

      case vokabeln:
      {
        KEduVocVokabelnReader vokabelnReader(f);
        d->m_url.setFileName(i18n("Untitled"));
        read = vokabelnReader.readDoc(this);
        if (!read)
          errorMessage = vokabelnReader.errorMessage();
      }
      break;

      case csv:
      {
        KEduVocCsvReader csvReader(f);
        read = csvReader.readDoc(this);
        if (!read)
          errorMessage = csvReader.errorMessage();
      }
      break;

      case xdxf:
      {
        KEduVocXdxfReader xdxfReader(f);
        d->m_url.setFileName(i18n("Untitled"));
        read = xdxfReader.readDoc(this);
        if (!read)
          errorMessage = xdxfReader.errorMessage();
      }
      break;

      default:
      {
        KEduVocKvtmlReader kvtmlReader(f);
        read = kvtmlReader.readDoc(this);
        if (!read)
          errorMessage = kvtmlReader.errorMessage();
      }
    }

    QApplication::restoreOverrideCursor();

    if (!read) {
      QString msg = i18n("Could not open \"%1\"\n(Error reported: %2)", url.path(), errorMessage);
      KMessageBox::error(0, msg, i18n("Error Opening File"));
    }

    f->close();
    KIO::NetAccess::removeTempFile(temporaryFile);
  }
  return read;
}


bool KEduVocDocument::saveAs(const KUrl & url, FileType ft, const QString & generator)
{
  KUrl tmp(url);

  if (ft == automatic)
  {
    if (tmp.path().right(strlen("." KVTML_EXT)) == "." KVTML_EXT)
      ft = kvtml;
    else if (tmp.path().right(strlen("." CSV_EXT)) == "." CSV_EXT)
      ft = csv;
    else
    {
      return false;
    }
  }

  bool saved = false;

  while (!saved)
  {
    QFile f(tmp.path());

    if (!f.open(QIODevice::WriteOnly))
    {
      KMessageBox::error(0, i18n("<qt>Cannot write to file<br><b>%1</b></qt>", tmp.path()));
      return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    switch (ft) {
      case kvtml: {
        KEduVocKvtmlWriter kvtmlWriter(&f);
        saved = kvtmlWriter.writeDoc(this, generator);
      }
      break;

      case csv: {
        KEduVocCsvWriter csvWriter(&f);
        saved = csvWriter.writeDoc(this, generator);
      }
      break;

      default: {
        kError() << "kvcotrainDoc::saveAs(): unknown filetype" << endl;
      }
      break;
    }
    f.close();
    QApplication::restoreOverrideCursor();

    if (!saved) {
      QString msg = i18n("Could not save \"%1\"\nDo you want to try again?", tmp.path());
      int result = KMessageBox::warningContinueCancel(0, msg, i18n("Error Saving File"), KGuiItem(i18n("&Retry")));
      if (result == KMessageBox::Cancel) 
        return false;
    }
  }
  d->m_url = tmp;
  setModified(false);
  return true;
}

void KEduVocDocument::merge(KEduVocDocument *docToMerge, bool matchIdentifiers)
{
  if (docToMerge) {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QStringList new_names = docToMerge->lessonDescriptions();

    QStringList new_types = docToMerge->typeDescriptions();

    QStringList new_tenses = docToMerge->tenseDescriptions();

    QList<int> old_in_query = lessonsInQuery();
    QList<int> new_in_query = docToMerge->lessonsInQuery();

    QStringList new_usages = docToMerge->usageDescriptions();

    int lesson_offset = d->m_lessonDescriptions.count();
    for (int i = 0; i < new_names.count(); i++) {
      d->m_lessonDescriptions.append(new_names[i]);
    }

    for (int i = 0; i < new_in_query.count(); i++)
      old_in_query.append(new_in_query[i] + lesson_offset);
    setLessonsInQuery(old_in_query);

    int types_offset = d->m_typeDescriptions.count();
    for (int i = 0; i < new_types.count(); i++) {
      d->m_typeDescriptions.append(new_types[i]);
    }

    int tenses_offset = d->m_tenseDescriptions.count();
    for (int i = 0; i < new_tenses.count(); i++) {
      d->m_tenseDescriptions.append(new_tenses[i]);
    }

    int usages_offset = d->m_usageDescriptions.count();
    for (int i = 0; i < new_usages.count(); i++) {
      d->m_usageDescriptions.append(new_usages[i]);
    }

    bool equal = true;
    if (originalIdentifier() != docToMerge->originalIdentifier())
      equal = false;
    for (int i = 1; i < identifierCount(); i++)
      if (identifier(i) != docToMerge->identifier(i))
        equal = false;

    if (!matchIdentifiers)
      equal = true; ///@todo massive cheating, problem if docToMerge has more identifiers than this

    if (equal) {   // easy way: same language codes, just append

      for (int i = 0; i < docToMerge->entryCount(); i++) {
        KEduVocExpression *expr = docToMerge->entry(i);

        expr->setLesson(expr->lesson() + lesson_offset);

        for (int lang = 0; lang <= expr->translationCount(); lang++) {
          QString t = expr->type(lang);
          // adjust type offset
          if (!t.isEmpty() && t.left(1) == QM_USER_TYPE) {
            QString t2;
            t.remove(0, 1);
            t2.setNum(t.toInt() + types_offset);
            t2.prepend(QM_USER_TYPE);
            expr->setType (lang, t2);
          }

          t = expr->usageLabel(lang);
          // adjust usage offset
          QString tg;
          if (!t.isEmpty()) {
            QString t2;
            while (t.left(strlen(":")) == UL_USER_USAGE) {
              QString n;
              t.remove(0, 1);
              int next;
              if ((next = t.indexOf(":")) >= 0) {
                n = t.left(next);
                t.remove(0, next + 1);
              }
              else {
                n = t;
                t = "";
              }

              t2.setNum(n.toInt() + usages_offset);
              t2.prepend(UL_USER_USAGE);
              if (tg.length() == 0)
                tg = t2;
              else
                tg += ':' + t2;
            }

            if (tg.length() == 0)
              tg = t;
            else if (t.length() != 0)
              tg += ':' + t;

            expr->setUsageLabel (lang, tg);
          }

          KEduVocConjugation conj = expr->conjugation(lang);
          bool condirty = false;
          for (int ci = 0; ci < conj.entryCount(); ci++) {
            t = conj.getType(ci);
            if (!t.isEmpty() && t.left(1) == UL_USER_TENSE) {
              t.remove(0, strlen(UL_USER_TENSE));
              QString t2;
              t2.setNum(t.toInt() + tenses_offset);
              t2.prepend(UL_USER_TENSE);
              conj.setType(ci, t2);
              condirty = true;
            }
            if (condirty)
              expr->setConjugation(lang, conj);
          }
        }

        appendEntry(expr);
      }
      setModified();
    }
    else { // hard way: move entries around, most attributes get lost
      QList<int> move_matrix;
      QList<bool> cs_equal;
      QString s;

      for (int i = 0; i < qMax (identifierCount(), docToMerge->identifierCount()); i++)
        cs_equal.append(false);

      move_matrix.append(docToMerge->indexOfIdentifier(originalIdentifier()));
      for (int i = 1; i < identifierCount(); i++)
        move_matrix.append(docToMerge->indexOfIdentifier(identifier(i)));

      for (int j = 0; j < docToMerge->entryCount(); j++) {
        KEduVocExpression new_expr;
        KEduVocExpression *expr = docToMerge->entry(j);
        new_expr.setLesson(expr->lesson()+lesson_offset);

        for (int i = 0; i < move_matrix.count(); i++) {
          int lpos = move_matrix[i];
          if (lpos >= 0) {

            if (lpos == 0)
              s = expr->original();
            else
              s = expr->translation(lpos);

            if (!cs_equal[lpos]) {
              cs_equal[lpos] = true;
              QString id = lpos == 0 ? originalIdentifier() : identifier(lpos);
            }

            if (i == 0)
              new_expr.setOriginal(s);
            else
              new_expr.setTranslation(i, s);
            QString r = expr->remark(lpos);
            new_expr.setRemark (i, r);

            QString t = expr->type(lpos);
            if (!t.isEmpty() && t.left(1) == QM_USER_TYPE) {
              QString t2;
              t.remove(0, 1);
              t2.setNum(t.toInt() + types_offset);
              t2.prepend(QM_USER_TYPE);
              new_expr.setType(i, t2);
            }

            t = expr->usageLabel(lpos);
            if (!t.isEmpty() && t.left(1) == QM_USER_TYPE) {
              QString t2;
              t.remove(0, 1);
              t2.setNum(t.toInt() + usages_offset);
              t2.prepend(QM_USER_TYPE);
              new_expr.setUsageLabel(i, t2);
            }

            KEduVocConjugation conj = expr->conjugation(lpos);
            for (int ci = 0; ci < conj.entryCount(); ci++) {
              t = conj.getType(ci);
              if (!t.isEmpty() && t.left(1) == QM_USER_TYPE) {
                t.remove (0, strlen(QM_USER_TYPE));
                QString t2;
                t2.setNum(t.toInt() + tenses_offset);
                t2.prepend(QM_USER_TYPE);
                conj.setType(ci, t2);
              }
            }

          }
        }
        // only append if entries are used
        bool used = !new_expr.original().isEmpty();
        for (int i = 1; i < identifierCount(); i++)
          if (!new_expr.translation(i).isEmpty())
            used = true;

        if (used) {
          appendEntry(&new_expr);
          setModified();
        }
      }
    }
  }
  QApplication::restoreOverrideCursor();
}

KEduVocExpression *KEduVocDocument::entry(int index)
{
  if (index < 0 || index >= d->m_vocabulary.size() )
    return 0;
  else
    return &d->m_vocabulary[index];
}


void KEduVocDocument::removeEntry(int index)
{
  if (index >= 0 && index < d->m_vocabulary.size() )
    d->m_vocabulary.removeAt( index );
}


int KEduVocDocument::indexOfIdentifier(const QString &lang) const
{
  QStringList::const_iterator first = d->m_identifiers.begin();
  int count = 0;
  while (first != d->m_identifiers.end()) {
    if (*first == lang)
      return count;
    first++;
    count++;
  }
  return -1;
}


QString KEduVocDocument::identifier(int index) const
{
  if (index >= d->m_identifiers.size() || index < 1 )
    return "";
  else
    return d->m_identifiers[index];
}


void KEduVocDocument::setIdentifier(int idx, const QString &id)
{
  if (idx < d->m_identifiers.size() && idx >= 1 ) {
    d->m_identifiers[idx] = id;
  }
}


QString KEduVocDocument::typeName (int index) const
{
  if (index >= d->m_typeDescriptions.count())
    return "";
  else
    return d->m_typeDescriptions[index];
}


void KEduVocDocument::setTypeName(int idx, QString &id)
{
  if (idx >= d->m_typeDescriptions.size())
    for (int i = d->m_typeDescriptions.size(); i <= idx; i++)
      d->m_typeDescriptions.push_back ("");

  d->m_typeDescriptions[idx] = id;
}


QStringList KEduVocDocument::typeDescriptions() const
{
  return d->m_typeDescriptions;
}


void KEduVocDocument::setTypeDescriptions(const QStringList &names)
{
  d->m_typeDescriptions = names;
}


QString KEduVocDocument::tenseName(int index) const
{
  if (index >= d->m_tenseDescriptions.size())
    return "";
  else
    return d->m_tenseDescriptions[index];
}


void KEduVocDocument::setTenseName(int idx, QString &id)
{
  if (idx >= d->m_tenseDescriptions.size())
    for (int i = d->m_tenseDescriptions.size(); i <= idx; i++)
      d->m_tenseDescriptions.push_back ("");

  d->m_tenseDescriptions[idx] = id;
}


QStringList KEduVocDocument::tenseDescriptions() const
{
  return d->m_tenseDescriptions;
}


void KEduVocDocument::setTenseDescriptions(const QStringList &names)
{
  d->m_tenseDescriptions = names;
}


QString KEduVocDocument::usageName(int index) const
{
  if (index >= d->m_usageDescriptions.size())
    return "";
  else
    return d->m_usageDescriptions[index];
}


void KEduVocDocument::setUsageName(int idx, QString &id)
{
  if (idx >= d->m_usageDescriptions.size())
    for (int i = d->m_usageDescriptions.size(); i <= idx; i++)
      d->m_usageDescriptions.push_back ("");

  d->m_usageDescriptions[idx] = id;
}


QStringList KEduVocDocument::usageDescriptions() const
{
  return d->m_usageDescriptions;
}


void KEduVocDocument::setUsageDescriptions(const QStringList &names)
{
  d->m_usageDescriptions = names;
}


void KEduVocDocument::setConjugation(int idx, const KEduVocConjugation &con)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if (d->m_conjugations.size() <= idx )
    for (int i = d->m_conjugations.size(); i < idx+1; i++)
      d->m_conjugations.push_back (KEduVocConjugation());

  d->m_conjugations[idx] = con;
}


int KEduVocDocument::conjugationCount() const
{
  return d->m_conjugations.count();
}


KEduVocConjugation KEduVocDocument::conjugation (int idx) const
{
  if (idx >= d->m_conjugations.size() || idx < 0) {
    return KEduVocConjugation();
  }
  else {
    return d->m_conjugations[idx];
  }
}


void KEduVocDocument::setArticle(int idx, const KEduVocArticle &art)
{
  if ( idx < 0) return;

  // extend conjugation with empty elements
  if (d->m_articles.size() <= idx )
    for (int i = d->m_articles.size(); i < idx+1; i++)
      d->m_articles.push_back (KEduVocArticle());

  d->m_articles[idx] = art;
}


KEduVocArticle KEduVocDocument::article (int idx) const
{
  if (idx >= d->m_articles.size() || idx < 0) {
    return KEduVocArticle();
  }
  else {
    return d->m_articles[idx];
  }
}


int KEduVocDocument::articleCount() const
{
  return d->m_articles.count();
}


int KEduVocDocument::sizeHint (int idx) const
{
  if (idx < 0) {
    idx = -idx;
    if (idx >= d->m_extraSizeHints.size() )
      return 80; // make a good guess about column size
    else {
//      cout << "gsh " << idx << "  " << extraSizehints[idx] << endl;
      return d->m_extraSizeHints[idx];
    }
  }
  else {
    if (idx >= d->m_sizeHints.size() )
      return 150; // make a good guess about column size
    else {
//      cout << "gsh " << idx << "  " << sizehints[idx] << endl;
      return d->m_sizeHints[idx];
    }
  }
}


void KEduVocDocument::setSizeHint (int idx, const int width)
{
//  cout << "ssh " << idx << "  " << width << endl;
  if (idx < 0) {
    idx = -idx;
    if (idx >= d->m_extraSizeHints.size()) {
      for (int i = d->m_extraSizeHints.size(); i <= idx; i++)
        d->m_extraSizeHints.push_back (80);
    }
    d->m_extraSizeHints[idx] = width;

  }
  else {
    if (idx >= d->m_sizeHints.size()) {
      for (int i = d->m_sizeHints.size(); i <= idx; i++)
        d->m_sizeHints.push_back (150);
    }
    d->m_sizeHints[idx] = width;
  }
}


void KEduVocDocument::removeIdentifier(int index)
{
  if (index < d->m_identifiers.size() && index >= 1 )
  {
    d->m_identifiers.removeAt(index);
    for (int i = 0; i < d->m_vocabulary.count(); i++)
      d->m_vocabulary[i].removeTranslation(index);
  }
}


QString KEduVocDocument::originalIdentifier() const
{
  if (d->m_identifiers.size() > 0)
    return d->m_identifiers[0];
  else
    return "";
}


void KEduVocDocument::setOriginalIdentifier(const QString &id)
{
  if (d->m_identifiers.size() > 0) {
    d->m_identifiers[0] = id;
  }
}


bool KEduVocDocument::sort(int index, Qt::SortOrder order)
{
  bool result = false;
  if (d->m_sortingEnabled && index < identifierCount())
  {
    if (d->m_sortIdentifier.count() < d->m_identifiers.count())
      for (int i = d->m_sortIdentifier.count(); i < d->m_identifiers.count(); i++)
          d->m_sortIdentifier.append(false);

    d->m_sortIdentifier[index] = (order == Qt::AscendingOrder);
    result = sort(index);
  }
  return result;
}

bool KEduVocDocument::sort(int index)
{
  bool result = false;
  if (d->m_sortingEnabled && index < identifierCount())
  {
    if (d->m_sortIdentifier.count() < d->m_identifiers.count())
      for (int i = d->m_sortIdentifier.count(); i < d->m_identifiers.count(); i++)
          d->m_sortIdentifier.append(false);

    KEduVocDocumentSortHelper sh(index, d->m_sortIdentifier[index] ? Qt::AscendingOrder : Qt::DescendingOrder);
    qSort(d->m_vocabulary.begin(), d->m_vocabulary.end(), sh);
    d->m_sortIdentifier[index] = !d->m_sortIdentifier[index];
    result = d->m_sortIdentifier[index];
  }
  return result;
}


bool KEduVocDocument::sortByLessonAlpha ()
{
  ///@todo remove?
  return false;
}


bool KEduVocDocument::sortByLessonIndex ()
{
  ///@todo remove?
  return false;
}


void KEduVocDocument::setSortingEnabled(bool enable)
{
  d->m_sortingEnabled = enable;
}


bool KEduVocDocument::isSortingEnabled() const
{
  return d->m_sortingEnabled;
}


bool KEduVocDocument::isModified() const
{
  return d->m_dirty;
}


bool KEduVocDocument::leitnerSystemActive()
{
  return d->m_activeLeitnerSystem;
}

void KEduVocDocument::setLeitnerSystemActive( bool yes )
{
	if( yes )
	{
		if (d->m_leitnerSystem == 0)
			createStandardLeitnerSystem(); //if nothing is loaded yet

		d->m_activeLeitnerSystem = true;
	}
	else if( !yes )
		d->m_activeLeitnerSystem = false;
}

void KEduVocDocument::createStandardLeitnerSystem()
{
	LeitnerSystem* tmpSystem = new LeitnerSystem();
	QString name = "Standard";

	tmpSystem->setSystemName( name );
	tmpSystem->insertBox( "Box 1" );
	tmpSystem->insertBox( "Box 2" );
	tmpSystem->insertBox( "Box 3" );
	tmpSystem->insertBox( "Box 4" );
	tmpSystem->insertBox( "Box 5" );

	tmpSystem->setCorrectBox( "Box 1", "Box 2" );
	tmpSystem->setWrongBox( "Box 1", "Box 1" );

	tmpSystem->setCorrectBox( "Box 2", "Box 3" );
	tmpSystem->setWrongBox( "Box 2", "Box 1" );

	tmpSystem->setCorrectBox( "Box 3", "Box 4" );
	tmpSystem->setWrongBox( "Box 3", "Box 1" );

	tmpSystem->setCorrectBox( "Box 4", "Box 5" );
	tmpSystem->setWrongBox( "Box 4", "Box 1" );

	tmpSystem->setCorrectBox( "Box 5", "Box 1" );
	tmpSystem->setWrongBox( "Box 5", "Box 1" );

	d->m_leitnerSystem = tmpSystem;
}

void KEduVocDocument::setLeitnerSystem( LeitnerSystem* system )
{
  d->m_leitnerSystem = system;
}

LeitnerSystem* KEduVocDocument::leitnerSystem()
{
  return d->m_leitnerSystem;
}


int KEduVocDocument::entryCount() const
{
  return d->m_vocabulary.count();
}


void KEduVocDocument::resetEntry(int index, int lesson)
{
  for (int i = 0; i < d->m_vocabulary.count(); i++)
    if (/*lesson == 0 ||*/ lesson == d->m_vocabulary[i].lesson())
    {
      d->m_vocabulary[i].setGrade(index, KV_NORM_GRADE, false);
      d->m_vocabulary[i].setGrade(index, KV_NORM_GRADE, true);
      d->m_vocabulary[i].setQueryCount(index, 0, true);
      d->m_vocabulary[i].setQueryCount(index, 0, false);
      d->m_vocabulary[i].setBadCount(index, 0, true);
      d->m_vocabulary[i].setBadCount(index, 0, false);
      QDateTime dt;
      dt.setTime_t(0);
      d->m_vocabulary[i].setQueryDate(index, dt, true);
      d->m_vocabulary[i].setQueryDate(index, dt, false);
    }
}


int KEduVocDocument::identifierCount() const
{
  return d->m_identifiers.count();  // org + translations
}


void KEduVocDocument::appendIdentifier(const QString & id)
{
  d->m_identifiers.append(id);
}


QString KEduVocDocument::lessonDescription(int idx) const
{
  if (idx == 0)
    return i18n("<no lesson>");

  if (idx <= 0 || idx > d->m_lessonDescriptions.size() )
    return "";

  return d->m_lessonDescriptions[idx-1];
}

int KEduVocDocument::lessonIndex(const QString &description) const
{
  return d->m_lessonDescriptions.indexOf(description) +1;
}


int KEduVocDocument::appendLesson(const QString &lessonName)
{
  d->m_lessonDescriptions.append(lessonName);
  return d->m_lessonDescriptions.count(); // counting from 1
}


void KEduVocDocument::renameLesson(const int lessonIndex, const QString &lessonName)
{
  d->m_lessonDescriptions.replace(lessonIndex-1, lessonName); // counting from 1
}


bool KEduVocDocument::lessonInQuery(int lessonIndex) const
{
  if (d->m_lessonsInQuery.contains(lessonIndex))
    return true;
  else
    return false;
}


void KEduVocDocument::addLessonToQuery(int lessonIndex)
{
  if(!lessonInQuery(lessonIndex))
    d->m_lessonsInQuery.append(lessonIndex);
}


void KEduVocDocument::removeLessonFromQuery(int lessonIndex)
{
  if(lessonInQuery(lessonIndex))
    d->m_lessonsInQuery.removeAt(d->m_lessonsInQuery.indexOf(lessonIndex));
}


QList<int> KEduVocDocument::lessonsInQuery() const
{
  return d->m_lessonsInQuery;
}


void KEduVocDocument::setLessonsInQuery(const QList<int> &lesson_iq)
{
  d->m_lessonsInQuery = lesson_iq;
}

KUrl KEduVocDocument::url() const
{
  return d->m_url;
}


void KEduVocDocument::setUrl(const KUrl& url)
{
  d->m_url = url;
}


QString KEduVocDocument::title() const
{
  if (d->m_title.isEmpty())
    return d->m_url.fileName();
  else
    return d->m_title;
}


QString KEduVocDocument::author() const
{
  return d->m_author;
}


QString KEduVocDocument::license() const
{
  return d->m_license;
}


QString KEduVocDocument::documentRemark() const
{
  return d->m_remark;
}


void KEduVocDocument::queryIdentifier(QString &org, QString &trans) const
{
  org = d->m_queryorg;
  trans = d->m_querytrans;
}


void KEduVocDocument::setQueryIdentifier(const QString &org, const QString &trans)
{
  d->m_queryorg = org;
  d->m_querytrans = trans;
}


void KEduVocDocument::setTitle(const QString & title)
{
  d->m_title = title.simplified();
}


void KEduVocDocument::setAuthor(const QString & s)
{
  d->m_author = s.simplified();
}


void KEduVocDocument::setLicense(const QString & s)
{
  d->m_license = s.simplified();
}


void KEduVocDocument::setDocumentRemark(const QString & s)
{
  d->m_remark = s.simplified();
}


void KEduVocDocument::setGenerator(const QString & generator)
{
  d->m_generator = generator;
}


QString KEduVocDocument::generator() const
{
  return d->m_generator;
}


QString KEduVocDocument::version() const
{
  return d->m_version;
}


void KEduVocDocument::setVersion(const QString & vers)
{
  d->m_version = vers;
}


int KEduVocDocument::currentLesson() const
{
  return d->m_currentLesson;
}


void KEduVocDocument::setCurrentLesson(int lesson)
{
  d->m_currentLesson = lesson;
}


QStringList KEduVocDocument::lessonDescriptions() const
{
  return d->m_lessonDescriptions;
}


int KEduVocDocument::lessonCount() const
{
  return d->m_lessonDescriptions.count();
}

bool KEduVocDocument::deleteLesson(int lessonIndex, int deleteMode)
{  // too bad we count from one!
  lessonIndex++;
  for (int ent = 0; ent < entryCount(); ent++) {
    if (entry(ent)->lesson() == lessonIndex) {
      if (deleteMode == DeleteEmptyLesson)
        return false; // stop if there are vocabs left in the lesson
      if (deleteMode == DeleteEntriesAndLesson)
        // delete entries of this lesson with this lesson
        removeEntry(ent);
    }
  }//for entries

  // for all above this lesson number - reduce lesson by one.
  for (int ent = 0; ent < entryCount(); ent++) {
    if (entry(ent)->lesson() > lessonIndex) {
      entry(ent)->setLesson(entry(ent)->lesson()-1);
    }
  } // reduce lesson

  // finally just remove the lesson name
  d->m_lessonDescriptions.removeAt(lessonIndex-1); // because of the damned 0 arghh

  int currentInQuery = d->m_lessonsInQuery.indexOf(lessonIndex);
  if(currentInQuery != -1)
    d->m_lessonsInQuery.removeAt(currentInQuery);

  // move query entries
  for(int queryLesson = 0; queryLesson < d->m_lessonsInQuery.count(); queryLesson++)
  {
    if(d->m_lessonsInQuery.at(queryLesson) > lessonIndex)
      d->m_lessonsInQuery.replace(queryLesson, d->m_lessonsInQuery.at(queryLesson)-1);
  }
  return true;
}


void KEduVocDocument::setLessonDescriptions(const QStringList &names)
{
  d->m_lessonDescriptions = names;
}

void KEduVocDocument::moveLesson(int from, int to)
{
///@todo move in query as well!
  // still counting from 1
  d->m_lessonDescriptions.move(from -1, to -1);

  /*
  to > from?
    lesson >= from && lesson < to: lesson++
  to < from?
    lesson >= to && lesson < from: lesson++
  */
  for (int ent = 0; ent < entryCount(); ent++) {
    // put from directly to to
    if (entry(ent)->lesson() == from) {
      entry(ent)->setLesson(to);
    }
    else
    {
      if(to > from)
      {
        if(entry(ent)->lesson() >= from && entry(ent)->lesson() < to)
          entry(ent)->setLesson(entry(ent)->lesson()-1);
      }
      else
      {
        if(entry(ent)->lesson() >= to && entry(ent)->lesson() < from)
          entry(ent)->setLesson(entry(ent)->lesson()+1);
      }
    }
  }
}

int KEduVocDocument::search(const QString &substr, int id, int first, int last, bool word_start)
{
  if (last >= entryCount() || last < 0)
    last = entryCount();

  if (first < 0)
    first = 0;

  if (id >= identifierCount() || last < first)
    return -1;

  if (id == 0) {
    for (int i = first; i < last; i++) {
      if (word_start) {
        if (entry(i)->original().indexOf(substr, 0, Qt::CaseInsensitive) == 0)  // case insensitive
          return i;
      }
      else {
        if (entry(i)->original().indexOf(substr, 0, Qt::CaseInsensitive) > -1)  // case insensitive
          return i;
      }
    }
  }
  else {
    for (int i = first; i < last; i++) {
      if (word_start) {
        if (entry(i)->translation(id).indexOf(substr, 0, Qt::CaseInsensitive) == 0) // case insensitive
          return i;
      }
      else {
        if (entry(i)->translation(id).indexOf(substr, 0, Qt::CaseInsensitive) > -1) // case insensitive
          return i;
      }
    }
  }
  return -1;
}


QString KEduVocDocument::csvDelimiter() const
{
  return d->m_csvDelimiter;
}


void KEduVocDocument::setCsvDelimiter(const QString &delimiter)
{
  d->m_csvDelimiter = delimiter;
}


class ExpRef {

public:
  ExpRef() {}
  ExpRef (KEduVocExpression *_exp, int _idx)
  {
    idx    = _idx;
    exp    = _exp;
  }

  bool operator< (const ExpRef& y) const
  {
    QString s1 = exp->original();
    QString s2 = y.exp->original();
    int cmp = QString::compare(s1.toUpper(), s2.toUpper());
    if (cmp != 0)
      return cmp < 0;

    for (int i = 1; i < exp->translationCount(); i++) {

      s1 = exp->translation(i);
      s2 = y.exp->translation(i);
      cmp = QString::compare(s1.toUpper(), s2.toUpper() );
      if (cmp != 0)
        return cmp < 0;
    }
    return cmp < 0;
  }

  int idx;
  KEduVocExpression *exp;
};

typedef QList<ExpRef> ExpRefList;

int KEduVocDocument::cleanUp()
{
  int count = 0;
  KEduVocExpression *kve1, *kve2;
  ExpRefList shadow;
  QList<int> to_delete;

  for (int i = 0; i < d->m_vocabulary.count(); i++)
    shadow.append(ExpRef (entry(i), i));
  qStableSort(shadow.begin(), shadow.end());

  int ent_no = 0;
  int ent_percent = d->m_vocabulary.size () / 100;
  float f_ent_percent = d->m_vocabulary.size () / 100.0;
  emit progressChanged(this, 0);

  for (int i = shadow.size() - 1; i > 0; i--) {
    kve1 = shadow[i].exp;
    kve2 = shadow[i - 1].exp;

    ent_no++;
    if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
      emit progressChanged(this, (int)((ent_no / f_ent_percent) / 2.0));

    bool equal = true;
    if (kve1->original() == kve2->original() ) {
      for (int l = 1; equal && l < identifierCount(); l++ )
        if (kve1->translation(l) != kve2->translation(l))
          equal = false;

      if (equal) {
        to_delete.append(shadow[i - 1].idx);
        count++;
      }
    }
  }

  // removing might take very long
  ent_no = 0;
  ent_percent = to_delete.size () / 100;
  f_ent_percent = to_delete.size () / 100.0;
  emit progressChanged(this, 0);

  qStableSort(to_delete.begin(), to_delete.end());

  for (int i = (int) to_delete.count() - 1; i >= 0; i--) {
    ent_no++;
    if (ent_percent != 0 && (ent_no % ent_percent) == 0 )
      emit progressChanged(this, (int)(50 + ent_no / f_ent_percent / 2.0));
    removeEntry(to_delete[i]);
    setModified();
  }

  return count;
}

void KEduVocDocument::shuffle()
{
  KRandomSequence rs;
  rs.randomize(d->m_vocabulary);
  setModified();
}


QString KEduVocDocument::pattern(Mode mode)
{
  static const struct SupportedFilter
  {
    bool reading;
    bool writing;
    const char* extensions;
    const char* description;
  } filters[] = {
    { true, true, "*.kvtml", I18N_NOOP("KDE Vocabulary Document") },
    { true, false, "*.wql", I18N_NOOP("KWordQuiz Document") },
    { true, false, "*.xml.qz *.pau.gz", I18N_NOOP("Pauker Lesson") },
    { true, false, "*.voc", I18N_NOOP("Vokabeltrainer") },
    { true, false, "*.xdxf", I18N_NOOP("XML Dictionary Exchange Format") },
    { true, true, "*.csv", I18N_NOOP("Text") },
    // last is marker for the end, do not remove it
    { false, false, 0, 0 }
  };
  QStringList newfilters;
  QStringList allext;
  for (int i = 0; filters[i].extensions; ++i) {
    if ((mode == Reading && filters[i].reading) ||
        (mode == Writing && filters[i].writing)) {
      newfilters.append(QLatin1String(filters[i].extensions) + '|' + i18n(filters[i].description));
      allext.append(QLatin1String(filters[i].extensions));
    }
  }
  newfilters.prepend(allext.join(" ") + '|' + i18n("All supported documents"));
  return newfilters.join("\n");
}


#include "keduvocdocument.moc"
