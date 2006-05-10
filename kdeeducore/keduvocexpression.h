/***************************************************************************
                        Vocabulary Expression for KDE Edu
    -----------------------------------------------------------------------
    copyright            : (C) 1999-2001 Ewald Arnold
                           (C) 2001 The KDE-EDU team
                           (C) 2005-2006 Peter Hedlund
    email                : peter.hedlund@kdemail.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEDUVOCEXPRESSION_H
#define KEDUVOCEXPRESSION_H

#include <libkdeedu_core_export.h>

#define KV_MAX_GRADE       7
#define KV_MIN_GRADE       0

#define KV_NORM_GRADE      0       // not queried yet
#define KV_NORM_COLOR      Qt::black
#define KV_NORM_TEXT       I18N_NOOP("Not Queried Yet")

#define KV_LEV1_GRADE      1
#define KV_LEV1_TEXT       I18N_NOOP("Level 1")

#define KV_LEV2_GRADE      2
#define KV_LEV2_TEXT       I18N_NOOP("Level 2")

#define KV_LEV3_GRADE      3
#define KV_LEV3_TEXT       I18N_NOOP("Level 3")

#define KV_LEV4_GRADE      4
#define KV_LEV4_TEXT       I18N_NOOP("Level 4")

#define KV_LEV5_GRADE      5
#define KV_LEV5_TEXT       I18N_NOOP("Level 5")

#define KV_LEV6_GRADE      6
#define KV_LEV6_TEXT       I18N_NOOP("Level 6")

#define KV_LEV7_GRADE      7
#define KV_LEV7_TEXT       I18N_NOOP("Level 7")

#include <QList>
#include <QStringList>
#include <QDateTime>

#include "keduvocgrammar.h"
#include "keduvocmultiplechoice.h"

typedef signed char grade_t;
typedef unsigned short count_t;

/**
  This class contains one vocabulary expression as an original with one or more
  translations
  */

class EDUCORE_EXPORT KEduVocExpression
{
public:

  /** default constructor for an empty vocabulary expression
   */
  KEduVocExpression ();

  /** Constructor for a vocabulary expression with an original
   *
   * @param expression       original
   * @param lesson           lesson number, 0 for none
   */
  KEduVocExpression(const QString & expression, int lesson = 0);

  /** Constructor for a vocabulary expression with an original and one or more translations
   *
   * @param expression       expression
   * @param separator        expression will be split into an original and one or more translations using separator
   * @param lesson           lesson number, 0 for none
   */
  KEduVocExpression(const QString & expression, const QString & separator, int lesson = 0);

  /** adds a new translation of this entry
   * @param expression       translation
   * @param grade            grade of knowledge of this translation
   * @param reverseGrade     dito, in opposite direction
   */
  void addTranslation(const QString & expression, grade_t grade = KV_NORM_GRADE, grade_t reverseGrade = KV_NORM_GRADE);

  /** removes translation
   *
   * @param index            number of translation 1..x
   */
  void removeTranslation(int index);

  /** returns index of lesson (0 = none)
   */
  int lesson() const;

  /** sets index of lesson (0 = none)
   */
  void setLesson(int l);

  /** returns original expression of this entry
   */
  QString original() const;

  /** sets original expression of this entry
   */
  void setOriginal(const QString & expression);

  /** returns number of max. translations of all expressions
   */
  int numTranslations() const;

  /** returns flag if entry is "selected" for queries
   */
  bool isInQuery() const { return m_inQuery; }

  /** set entry "selected"
   */
  void setInQuery(bool flag = true) { m_inQuery = flag; }

  /** returns flag if entry is activated for queries
   */
  bool isActive() const  {return m_active; }

  /** set entry active (enabled for queries)
   */
  void setActive(bool flag = true) { m_active = flag; }

  /** returns translation of this expression
   *
   * @param index            number of translation
   * @return                 expression or "" if no translation available
   */
  QString translation(int index) const;

  /** sets translation of this expression
   *
   * @param index            number of translation
   * @param expression       expression of this index
   */
  void setTranslation(int index, const QString & expression);

  /** sets pronounciation of this expression
   *
   * @param index            index of expression
   * @param expression       pronounciation of this index
   */
  void setPronounciation(int index, const QString & expression);

  /** returns pronounciation of this expression
   *
   * @param index            index of expression
   * @return                 pronunciation or "" if none available
   */
  QString pronounciation(int index) const;

  /** returns remarks of this expression
   *
   * @param index            index of expression
   * @return                 remark or "" if no remark available
   */
  QString remark(int index) const;

  /** sets remark of this expression
   *
   * @param index            index of expression
   * @param expr             remark of this index
   */
  void setRemark(int index, const QString & expr);


  /** sets false friend of this expression
   *
   * @param index            index of expression
   * @param expression       false friend of this index
   * @param reverse          if true, represents the direction translation to original
   */
  void setFauxAmi(int index, const QString & expression, bool reverse = false);


  /** returns false friend of this expression
   *
   * @param index            index of expression
   * @param reverse          if true, represents the direction translation to original
   * @return                 false friend or "" if no string available
   */
  QString fauxAmi(int index, bool reverse = false) const;

  /** sets synonym this expression
   *
   * @param index            index of expression
   * @param expression       synonym of this index
   */
  void setSynonym(int index, const QString & expression);


  /** returns synonym of this expression
   *
   * @param index            index of expression
   * @return                 synonym or "" if no string available
   */
  QString synonym(int index) const;

  /** sets example this expression
   *
   * @param index            index of expression
   * @param expression       example of this index
   */
  void setExample(int index, const QString & expression);


  /** returns example of this expression
   *
   * @param index            index of expression
   * @return                 example or "" if no string available
   */
  QString example(int index) const;

  /** sets usage label this expression
   *
   * @param index            index of expression
   * @param usage            usage label of this index
   */
  void setUsageLabel(int index, const QString & usage);


  /** returns usage label of this expression
   *
   * @param index            index of expression
   * @return                 usage or "" if no string available
   */
  QString usageLabel(int index) const;

  /** sets paraphrase of this expression
   *
   * @param index            index of expression
   * @param expression       paraphrase of this index
   */
  void setParaphrase(int index, const QString & expression);


  /** returns paraphrase of this expression
   *
   * @param index            index of expression
   * @return                 paraphrase or "" if no string available
   */
  QString paraphrase(int index) const;

  /** sets antonym this expression
   *
   * @param index            index of expression
   * @param expression       antonym of this index
   */
  void setAntonym(int index, const QString & expression);


  /** returns antonym of this expression
   *
   * @param index            index of expression
   * @return                 antonym or "" if no string available
   */
  QString antonym(int index) const;

  /** returns type of this expression
   *
   * @return                 type or "" if no type available
   */
  QString type(int index) const;

  /** all langs have same type ?
   *
   * @return                 true if all have same type
   */
  bool uniqueType () const;

  /** sets type of this expression
   *
   * @param index            index of type
   * @param type             type of this expression ("" = none)
   */
  void setType(int index, const QString & type);

  /** returns grade of given translation as string
   *
   * @param index            index of expression
   * @param reverse          if true, represents the direction translation to original
   * @return                 number of knowlegde: 0=known, x=numbers not knows
   */
  QString gradeStr(int index, bool reverse = false) const;

  /** sets grade of given translation
   *
   * @param index            index of translation
   * @param grade            number of knowlegde: 0=known, x=numbers not knows
   * @param reverse          if true, represents the direction translation to original
   */
  void setGrade(int index, grade_t grade, bool reverse = false);

  /** returns grade of given translation as int
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   * @return                 number of knowlegde: 0=known, x=numbers not knows
   */
  grade_t grade(int index, bool reverse = false) const;

  /** increments grade of given translation
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   */
  void incGrade(int index, bool reverse = false);

  /** decrements grade of given translation
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   */
  void decGrade(int index, bool reverse = false);

  /** returns last query date of given translation as int
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   */
  QDateTime queryDate(int index, bool reverse = false) const;

  /** set last query date of given translation as int
   *
   * @param index            index of translation
   * @param date             the new date
   * @param reverse          if true, represents the direction translation to original
   */
  void setQueryDate(int index, const QDateTime & date, bool reverse = false);

  /** returns conjugations if available
   *
   * @param index            index of translation
   */
  KEduVocConjugation conjugation(int index) const;

  /** sets conjugations
   *
   * @param index            index of translation
   * @param conjugation      conjugation block
   */
  void setConjugation(int index, const KEduVocConjugation & conjugation);

  /** returns comparison if available
   *
   * @param index            index of translation
   */
  KEduVocComparison comparison(int index) const;

  /** sets comparison
   *
   * @param index            index of translation
   * @param comparison       comparison block
   */
  void setComparison(int index, const KEduVocComparison & comparison);

  /** returns multiple choice if available
   *
   * @param index            index of multiple choice
   */
  KEduVocMultipleChoice multipleChoice(int index) const;

  /** sets multiple choice
   *
   * @param index            index of translation
   * @param mc               multiple choice block
   */
  void setMultipleChoice(int index, const KEduVocMultipleChoice & mc);

  /** returns query count of given translation as int
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   */
  count_t queryCount(int index, bool reverse = false) const;

  /** set query count of given translation as int
   *
   * @param index            index of translation
   * @param count            the new count
   * @param reverse          if true, represents the direction translation to original
   */
  void setQueryCount(int index, count_t count, bool reverse = false);

  /** returns bad query count of given translation as int
   *
   * @param index            index of translation
   * @param reverse          dito, in opposite direction
   */
  count_t badCount(int index, bool reverse = false) const;

  /** set bad query count of given translation as int
   *
   * @param index            index of translation
   * @param count            the new count
   * @param reverse          if true, represents the direction translation to original
   */
  void setBadCount(int index, count_t count, bool reverse = false);

  /** increment bad query count of given translation by 1
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   */
  void incBadCount(int index, bool reverse = false);

  /** increment query count of given translation by 1
   *
   * @param index            index of translation
   * @param reverse          if true, represents the direction translation to original
   */
  void incQueryCount(int index, bool reverse = false);

  /** sets the box of the Leitner system which actually contains the expression
   *
   * @param box		     the box's name
   */
  void setLeitnerBox(const QString & box);

  /** returns the name of the Leitner system's box actually containing the expression
   *
   * @return                 the box's name
   */
  QString& leitnerBox();

protected:

  void Init();

private:
  QString m_original;

  // all these vectors must be deleted in removeTranslation()
  QStringList m_expressionTypes;
  QStringList m_translations;
  QStringList m_remarks;
  QStringList m_usageLabels;
  QStringList m_paraphrases;
  QStringList m_fauxAmi;
  QStringList m_reverseFauxAmi;
  QStringList m_synonym;
  QStringList m_example;
  QStringList m_antonym;
  QStringList m_pronounciations;
  QList<grade_t> m_grades;
  QList<grade_t> m_reverseGrades;
  QList<count_t> m_queryCounts;
  QList<count_t> m_reverseQueryCounts;
  QList<count_t> m_badCounts;
  QList<count_t> m_reverseBadCounts;
  QList<QDateTime> m_queryDates;
  QList<QDateTime> m_reverseQueryDates;
  QList<KEduVocConjugation> m_conjugations;
  QList<KEduVocComparison> m_comparisons;
  QList<KEduVocMultipleChoice> m_multipleChoices;

  QString m_leitnerBox;
  int m_sortIndex;
  int m_lesson;
  bool m_inQuery;
  bool m_active;
};

#endif // KEduVocExpression_H

