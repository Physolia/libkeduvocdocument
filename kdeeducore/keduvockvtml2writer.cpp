/***************************************************************************
                   export a KEduVocDocument to a KVTML file
    -----------------------------------------------------------------------
    copyright           : (C) 2007 Jeremy Whiting <jeremy@scitools.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keduvockvtml2writer.h"

#include <QTextStream>
#include <QFile>

#include <kdebug.h>

#include "keduvocdocument.h"
#include "keduvocexpression.h"
#include "kvtmldefs.h"
#include "kvtml2defs.h"

KEduVocKvtml2Writer::KEduVocKvtml2Writer(QFile *file)
{
  // the file must be already open
  m_outputFile = file;
}


bool KEduVocKvtml2Writer::writeDoc(KEduVocDocument *doc, const QString &generator)
{
  bool first_expr = true;

  m_doc = doc;

  m_domDoc = QDomDocument("kvtml SYSTEM \"kvtml2.dtd\"");
  m_domDoc.appendChild(m_domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
  QDomElement domElementKvtml = m_domDoc.createElement("kvtml");
  m_domDoc.appendChild(domElementKvtml);

  domElementKvtml.setAttribute(KVTML_VERSION, (QString) "2.0");

  // information group
  QDomElement currentElement = m_domDoc.createElement(KVTML_INFORMATION);
  writeInformation(currentElement, generator);
  domElementKvtml.appendChild(currentElement);
  
  // identifiers
  currentElement = m_domDoc.createElement(KVTML_IDENTIFIERS);
  writeIdentifiers(currentElement);
  domElementKvtml.appendChild(currentElement);
  
  // types
  currentElement = m_domDoc.createElement(KVTML_TYPES);
  writeTypes(currentElement);
  if (currentElement.hasChildNodes())
  {
    domElementKvtml.appendChild(currentElement);
  }
  
  // tenses
  currentElement = m_domDoc.createElement(KVTML_TENSES);
  writeTenses(currentElement);
  if (currentElement.hasChildNodes())
  {
    domElementKvtml.appendChild(currentElement);
  }
  
  // usages
  currentElement = m_domDoc.createElement(KVTML_USAGES);
  writeUsages(currentElement);
  if (currentElement.hasChildNodes())
  {
    domElementKvtml.appendChild(currentElement);
  }
  
  // entries
  
  // lessons
  
  //** NOTE: everything below this point has not been updated to use kvtml2 format****
  //if (!writeLesson(m_domDoc, domElementKvtml))
  //  return false;

  //QList<KEduVocConjugation> conjugations;
  //for (int i = 0; i < m_doc->conjugationCount(); i++)
  //  conjugations.append(m_doc->conjugation(i));
  //if (conjugations.count() > 0) {
  //  if (!writeConjugHeader(m_domDoc, domElementKvtml, conjugations))
  //    return false;
  //}

  //if (!writeOption(m_domDoc, domElementKvtml))
  //  return false;

  //if (!writeType(m_domDoc, domElementKvtml))
  //  return false;

  //if (!writeTense(m_domDoc, domElementKvtml))
  //  return false;

  //if (!writeUsage(m_domDoc, domElementKvtml))
  //  return false;

  //QString q_org, q_trans;
  //m_doc->queryIdentifier(q_org, q_trans);

  //int entryCount = m_doc->entryCount();

  //for (int i = 0; i < entryCount; i++)
  //{
  //  KEduVocExpression *entry = m_doc->entry(i);
  //  QDomElement domElementExpression = m_domDoc.createElement(KV_EXPR);

  //  if (entry->lesson() != 0)
  //  {
  //    int lm = entry->lesson();
  //    if (lm > m_doc->lessonDescriptions().count())
  //    {
  //      // should not be
  //      kError() << "index of lesson member too high: " << lm << endl;
  //      lm = 0;
  //    }
  //    domElementExpression.setAttribute(KV_LESS_MEMBER, lm);
  //  }

  //  if (entry->isInQuery())
  //    domElementExpression.setAttribute(KV_SELECTED, 1);

  //  if (!entry->isActive())
  //    domElementExpression.setAttribute (KV_INACTIVE, 1);

  //  if (entry->uniqueType() && !entry->translation(0).type().isEmpty())
  //    domElementExpression.setAttribute (KV_EXPRTYPE, entry->translation(0).type());

  //  QDomElement domElementOriginal = m_domDoc.createElement(KV_ORG);
  //  if (first_expr)
  //  {
  //    // save space, only tell language in first entry
  //    QString s;
  //    domElementOriginal.setAttribute(KV_SIZEHINT, m_doc->sizeHint(0));

  //    s = m_doc->identifier(0).simplified();
  //    if (s.isEmpty() )
  //      s = "original";
  //    domElementOriginal.setAttribute(KV_LANG, s);
  //    if (s == q_org)
  //      domElementOriginal.setAttribute(KV_QUERY, (QString) KV_O);
  //    else if (s == q_trans)
  //      domElementOriginal.setAttribute(KV_QUERY, (QString) KV_T);
  //  }

  //  if (!entry->translation(0).comment().isEmpty() )
  //    domElementOriginal.setAttribute(KV_REMARK, entry->translation(0).comment());

  //  if (!entry->translation(0).synonym().isEmpty() )
  //    domElementOriginal.setAttribute(KV_SYNONYM, entry->translation(0).synonym());

  //  if (!entry->translation(0).example().isEmpty() )
  //    domElementOriginal.setAttribute(KV_EXAMPLE, entry->translation(0).example());

  //  if (!entry->translation(0).usageLabel().isEmpty() )
  //    domElementOriginal.setAttribute(KV_USAGE, entry->translation(0).usageLabel());

  //  if (!entry->translation(0).paraphrase().isEmpty() )
  //    domElementOriginal.setAttribute(KV_PARAPHRASE, entry->translation(0).paraphrase());

  //  if (!entry->translation(0).antonym().isEmpty() )
  //    domElementOriginal.setAttribute(KV_ANTONYM, entry->translation(0).antonym());

  //  if (!entry->translation(0).pronunciation().isEmpty() )
  //    domElementOriginal.setAttribute(KV_PRONUNCE, entry->translation(0).pronunciation());

  //  if (!entry->uniqueType() && !entry->translation(0).type().isEmpty())
  //    domElementOriginal.setAttribute(KV_EXPRTYPE, entry->translation(0).type());

  //  if (!writeMultipleChoice(m_domDoc, domElementOriginal, entry->translation(0).multipleChoice()))
  //    return false;

  //  QString s;
  //  QString entype = s = entry->translation(0).type();
  //  int pos;
  //  if (pos >= 0)
  //    entype = s.left(pos);
  //  else
  //    entype = s;

  //  if (entry->translation(0).conjugation().entryCount() > 0)
  //  {
  //    KEduVocConjugation conj = entry->translation(0).conjugation();
  //    if (!writeConjugEntry(m_domDoc, domElementOriginal, conj))
  //      return false;
  //  }
  //  else if (!entry->translation(0).comparison().isEmpty())
  //  {
  //    KEduVocComparison comp = entry->translation(0).comparison();
  //    if (!writeComparison(m_domDoc, domElementOriginal, comp))
  //      return false;
  //  }

  //  QDomText domTextOriginal = m_domDoc.createTextNode(entry->translation(0).translation());
  //  domElementOriginal.appendChild(domTextOriginal);
  //  domElementExpression.appendChild(domElementOriginal);

  //  int trans = 1;
  //  while (trans < m_doc->identifierCount())
  //  {
  //    QDomElement domElementTranslation = m_domDoc.createElement(KV_TRANS);
  //    if (first_expr)
  //    {
  //      // save space, only tell language in first entry
  //      QString s;
  //      domElementTranslation.setAttribute(KV_SIZEHINT, m_doc->sizeHint(trans));

  //      s = m_doc->identifier(trans).simplified();
  //      if (s.isEmpty() )
  //      {
  //        s.setNum(trans);
  //        s.prepend("translation ");
  //      }
  //      domElementTranslation.setAttribute(KV_LANG, s);
  //      if (s == q_org)
  //        domElementTranslation.setAttribute(KV_QUERY, (QString) KV_O);
  //      else if (s == q_trans)
  //        domElementTranslation.setAttribute(KV_QUERY, (QString) KV_T);
  //    }

  //    if (entry->translation(trans).gradeFrom(0).grade() != 0 || entry->translation(0).gradeFrom(trans).grade() != 0)
  //      domElementTranslation.setAttribute(KV_GRADE, QString::number(entry->translation(trans).gradeFrom(0).grade()) + ';' + QString::number(entry->translation(0).gradeFrom(trans).grade()));

  //    if (entry->translation(trans).gradeFrom(0).queryCount() != 0 || entry->translation(0).gradeFrom(trans).queryCount() != 0)
  //      domElementTranslation.setAttribute(KV_COUNT, QString::number(entry->translation(trans).gradeFrom(0).queryCount()) + ';' + QString::number(entry->translation(0).gradeFrom(trans).queryCount()));

  //    if (entry->translation(trans).gradeFrom(0).badCount() != 0 || entry->translation(0).gradeFrom(trans).badCount() != 0)
  //      domElementTranslation.setAttribute(KV_BAD, QString::number(entry->translation(trans).gradeFrom(0).badCount()) + ';' + QString::number(entry->translation(0).gradeFrom(trans).badCount()));

  //    if (entry->translation(trans).gradeFrom(0).queryDate().toTime_t() != 0 || entry->translation(0).gradeFrom(trans).queryDate().toTime_t() != 0)
  //      domElementTranslation.setAttribute(KV_DATE, QString::number(entry->translation(trans).gradeFrom(0).queryDate().toTime_t()) + ';' + QString::number(entry->translation(0).gradeFrom(trans).queryDate().toTime_t()));

  //    if (!entry->translation(trans).comment().isEmpty() )
  //      domElementTranslation.setAttribute(KV_REMARK, entry->translation(trans).comment());

  //    if (!entry->translation(trans).falseFriend(0).isEmpty() )
  //      domElementTranslation.setAttribute(KV_FAUX_AMI_F, entry->translation(trans).falseFriend(0));

  //    if (!entry->translation(0).falseFriend(trans).isEmpty() )
  //      domElementTranslation.setAttribute(KV_FAUX_AMI_T, entry->translation(0).falseFriend(trans));

  //    if (!entry->translation(trans).synonym().isEmpty() )
  //      domElementTranslation.setAttribute(KV_SYNONYM, entry->translation(trans).synonym());

  //    if (!entry->translation(trans).example().isEmpty() )
  //      domElementTranslation.setAttribute(KV_EXAMPLE, entry->translation(trans).example());

  //    if (!entry->translation(trans).usageLabel().isEmpty() )
  //      domElementTranslation.setAttribute(KV_USAGE, entry->translation(trans).usageLabel());

  //    if (!entry->translation(trans).paraphrase().isEmpty() )
  //      domElementTranslation.setAttribute(KV_PARAPHRASE, entry->translation(trans).paraphrase());

  //    if (!entry->translation(trans).antonym().isEmpty() )
  //      domElementTranslation.setAttribute(KV_ANTONYM, entry->translation(trans).antonym());

  //    if (!entry->translation(trans).pronunciation().isEmpty() )
  //      domElementTranslation.setAttribute(KV_PRONUNCE, entry->translation(trans).pronunciation());

  //    if (!entry->uniqueType() && !entry->translation(trans).type().isEmpty())
  //      domElementTranslation.setAttribute(KV_EXPRTYPE, entry->translation(trans).type());

  //    if (!writeMultipleChoice(m_domDoc, domElementTranslation, entry->translation(trans).multipleChoice()))
  //      return false;

  //    QString s;
  //    QString entype = s = entry->translation(0).type();
  //    int pos;
  //    if (pos >= 0)
  //      entype = s.left (pos);
  //    else
  //      entype = s;

  //    if (entry->translation(trans).conjugation().entryCount() > 0)
  //    {
  //      KEduVocConjugation conj = entry->translation(trans).conjugation();
  //      if (!writeConjugEntry(m_domDoc, domElementTranslation, conj))
  //        return false;
  //    }

  //    if (!entry->translation(trans).comparison().isEmpty())
  //    {
  //      KEduVocComparison comp = entry->translation(trans).comparison();
  //      if (!writeComparison(m_domDoc, domElementTranslation, comp))
  //        return false;
  //    }

  //    QDomText domTextTranslation = m_domDoc.createTextNode(entry->translation(trans).translation());
  //    domElementTranslation.appendChild(domTextTranslation);
  //    domElementExpression.appendChild(domElementTranslation);

  //    trans++;
  //  }

  //  domElementKvtml.appendChild(domElementExpression);

  //  first_expr = false;
  //}

  m_domDoc.appendChild(domElementKvtml);

  QTextStream ts(m_outputFile);
  m_domDoc.save(ts, 2);

  return true;
}

bool KEduVocKvtml2Writer::writeInformation(QDomElement &informationElement, const QString &generator)
{
  QDomElement currentElement;
  QDomText textNode;
  
  // generator
  informationElement.appendChild(newTextElement(KVTML_GENERATOR, generator));
  
  // title
  if (!m_doc->title().isEmpty())
  {
    informationElement.appendChild(newTextElement(KVTML_TITLE, m_doc->title()));
  }

  // author
  if (!m_doc->author().isEmpty())
  {
    informationElement.appendChild(newTextElement(KVTML_AUTHOR, m_doc->author()));
  }

  // license
  if (!m_doc->license().isEmpty())
  {
    informationElement.appendChild(newTextElement(KVTML_LICENSE, m_doc->license()));
  }

  // comment
  if (!m_doc->documentRemark().isEmpty())
  {
    informationElement.appendChild(newTextElement(KVTML_COMMENT, m_doc->documentRemark()));
  }
  
  return true;
}


bool KEduVocKvtml2Writer::writeIdentifiers(QDomElement &identifiersElement)
{
  for (int i = 0; i < m_doc->identifierCount(); ++i)
  {
    // create the node
    QDomElement identifier = m_domDoc.createElement(KVTML_IDENTIFIER);
    
    // set the id
    identifier.setAttribute(KVTML_ID, QString::number(i));
    
    // record the identifier as the locale for now 
    // TODO: when support for more parts of the identifier is in the document class (name, type, etc.) store those here as well
    identifier.appendChild(newTextElement(KVTML_LOCALE, m_doc->identifier(i)));
    
    // record articles
    QDomElement article = m_domDoc.createElement(KVTML_ARTICLE);
    writeArticle(article, i);
    if (article.hasChildNodes())
    {
      identifier.appendChild(article);
    }
    
    // record personalpronouns
    QDomElement personalpronouns = m_domDoc.createElement(KVTML_PERSONALPRONOUNS);
    writeConjugation(personalpronouns, m_doc->conjugation(i), QString());
    if (personalpronouns.hasChildNodes())
    {
      identifier.appendChild(personalpronouns);
    }
    
    // add this identifier to the group
    identifiersElement.appendChild(identifier);
  }
}

bool KEduVocKvtml2Writer::writeLesson(QDomDocument &domDoc, QDomElement &domElementParent)
{
  if (m_doc->lessonDescriptions().count() == 0)
    return true;

  QDomElement domElementLesson = domDoc.createElement(KV_LESS_GRP);
  domElementLesson.setAttribute(KV_SIZEHINT, m_doc->sizeHint(-1));
  int count = 1;

  foreach(QString lesson, m_doc->lessonDescriptions())
  {
    if (!lesson.isNull())
    {
      QDomElement domElementDesc = domDoc.createElement(KV_LESS_DESC);
      QDomText domTextDesc = domDoc.createTextNode(lesson);

      domElementDesc.setAttribute(KV_LESS_NO, count);
      if (m_doc->currentLesson() == count)
        domElementDesc.setAttribute(KV_LESS_CURR, 1);
      if (m_doc->lessonInQuery(count))
        domElementDesc.setAttribute(KV_LESS_QUERY, 1);

      domElementDesc.appendChild(domTextDesc);
      domElementLesson.appendChild(domElementDesc);
      count++;
    }
  }

  domElementParent.appendChild(domElementLesson);
  return true;
}


bool KEduVocKvtml2Writer::writeArticle(QDomElement &articleElement, int article)
{
  QDomElement definite = m_domDoc.createElement(KVTML_DEFINITE);
  QDomElement indefinite = m_domDoc.createElement(KVTML_INDEFINITE);
  QString def;
  QString indef;
  
  // male
  m_doc->article(article).getMale(&def, &indef);
  definite.appendChild(newTextElement(KVTML_MALE, def));
  indefinite.appendChild(newTextElement(KVTML_MALE, indef));
  
  // female
  m_doc->article(article).getFemale(&def, &indef);
  definite.appendChild(newTextElement(KVTML_FEMALE, def));
  indefinite.appendChild(newTextElement(KVTML_FEMALE, indef));
    
  // neutral
  m_doc->article(article).getNatural(&def, &indef);
  definite.appendChild(newTextElement(KVTML_NEUTRAL, def));
  indefinite.appendChild(newTextElement(KVTML_NEUTRAL, indef));

  articleElement.appendChild(definite);
  articleElement.appendChild(indefinite);
  return true;
}

bool KEduVocKvtml2Writer::writeTypes(QDomElement &typesElement)
{
  foreach(QString type, m_doc->typeDescriptions())
  {
    if (!(type.isNull()) )
    {
      typesElement.appendChild(newTextElement(KVTML_TYPE, type));
    }
  }

  return true;
}


bool KEduVocKvtml2Writer::writeTenses(QDomElement &tensesElement)
{
  foreach(QString tense, m_doc->tenseDescriptions())
  {
    if (!(tense.isNull())) 
    {
      tensesElement.appendChild(newTextElement(KVTML_TENSE, tense));
    }
  }

  return true;
}


bool KEduVocKvtml2Writer::writeUsages(QDomElement &usagesElement)
{
  foreach(QString usage, m_doc->usageDescriptions())
  {
    if (!(usage.isNull()))
    {
      usagesElement.appendChild(newTextElement(KVTML_USAGE, usage));
    }
  }

  return true;
}


bool KEduVocKvtml2Writer::writeComparison(QDomDocument &domDoc, QDomElement &domElementParent, const KEduVocComparison &comp)
/*
 <comparison>
   <l1>good</l1>
   <l2>better</l2>
   <l3>best</l3>
 </comparison>
*/
{
  if (comp.isEmpty())
    return true;

  QDomElement domElementComparison = domDoc.createElement(KV_COMPARISON_GRP);

  if (!comp.l1().isEmpty() )
  {
    QDomElement domElementL1 = domDoc.createElement(KV_COMP_L1);
    QDomText domTextL1 = domDoc.createTextNode(comp.l1());

    domElementL1.appendChild(domTextL1);
    domElementComparison.appendChild(domElementL1);
  }

  if (!comp.l2().isEmpty() )
  {
    QDomElement domElementL2 = domDoc.createElement(KV_COMP_L2);
    QDomText domTextL2 = domDoc.createTextNode(comp.l2());

    domElementL2.appendChild(domTextL2);
    domElementComparison.appendChild(domElementL2);
  }

  if (!comp.l3().isEmpty() )
  {
    QDomElement domElementL3 = domDoc.createElement(KV_COMP_L3);
    QDomText domTextL3 = domDoc.createTextNode(comp.l3());

    domElementL3.appendChild(domTextL3);
    domElementComparison.appendChild(domElementL3);
  }

  domElementParent.appendChild(domElementComparison);
  return true;
}


bool KEduVocKvtml2Writer::writeMultipleChoice(QDomDocument &domDoc, QDomElement &domElementParent, const KEduVocMultipleChoice &mc)
/*
 <multiplechoice>
   <mc1>good</mc1>
   <mc2>better</mc2>
   <mc3>best</mc3>
   <mc4>best 2</mc4>
   <mc5>best 3</mc5>
 </multiplechoice>
*/
{
  if (mc.isEmpty())
    return true;

  QDomElement domElementMC = domDoc.createElement(KV_MULTIPLECHOICE_GRP);

  if (!mc.choice(1).isEmpty() )
  {
    QDomElement domElementMC1 = domDoc.createElement(KV_MC_1);
    QDomText domTextMC1 = domDoc.createTextNode(mc.choice(1));

    domElementMC1.appendChild(domTextMC1);
    domElementMC.appendChild(domElementMC1);
  }

  if (!mc.choice(2).isEmpty() )
  {
    QDomElement domElementMC2 = domDoc.createElement(KV_MC_2);
    QDomText domTextMC2 = domDoc.createTextNode(mc.choice(2));

    domElementMC2.appendChild(domTextMC2);
    domElementMC.appendChild(domElementMC2);
  }

  if (!mc.choice(3).isEmpty() )
  {
    QDomElement domElementMC3 = domDoc.createElement(KV_MC_3);
    QDomText domTextMC3 = domDoc.createTextNode(mc.choice(3));

    domElementMC3.appendChild(domTextMC3);
    domElementMC.appendChild(domElementMC3);
  }

  if (!mc.choice(4).isEmpty() )
  {
    QDomElement domElementMC4 = domDoc.createElement(KV_MC_4);
    QDomText domTextMC4 = domDoc.createTextNode(mc.choice(4));

    domElementMC4.appendChild(domTextMC4);
    domElementMC.appendChild(domElementMC4);
  }

  if (!mc.choice(5).isEmpty() )
  {
    QDomElement domElementMC5 = domDoc.createElement(KV_MC_5);
    QDomText domTextMC5 = domDoc.createTextNode(mc.choice(5));

    domElementMC5.appendChild(domTextMC5);
    domElementMC.appendChild(domElementMC5);
  }

  domElementParent.appendChild(domElementMC);
  return true;
}

bool KEduVocKvtml2Writer::writeConjugation(QDomElement &conjugationElement, 
                        const KEduVocConjugation &conjugation, const QString &type)
{
  // first singular conjugations
  QString first = conjugation.pers1Singular(type);
  QString second = conjugation.pers2Singular(type);
  bool third_common = conjugation.pers3SingularCommon(type);
  QString third_male = conjugation.pers3MaleSingular(type);
  QString third_female = conjugation.pers3FemaleSingular(type);
  QString third_neutral = conjugation.pers3NaturalSingular(type);
  
  if (!first.isEmpty() || !second.isEmpty() || !third_female.isEmpty() ||
      !third_male.isEmpty() || !third_neutral.isEmpty())
  {
    QDomElement singular = m_domDoc.createElement(KVTML_SINGULAR);
    
    singular.appendChild(newTextElement(KVTML_1STPERSON, first));
    singular.appendChild(newTextElement(KVTML_2NDPERSON, second));
    
    if (third_common)
    {
      singular.appendChild(newTextElement(KVTML_COMMON, third_female));
    }
    else
    {
      singular.appendChild(newTextElement(KVTML_MALE, third_male));
      singular.appendChild(newTextElement(KVTML_FEMALE, third_female));
      singular.appendChild(newTextElement(KVTML_NEUTRAL, third_neutral));
    }
  }
  
  // now for plurals
  first = conjugation.pers1Plural(type);
  second = conjugation.pers2Plural(type);
  third_common = conjugation.pers3PluralCommon(type);
  third_male = conjugation.pers3MalePlural(type);
  third_female = conjugation.pers3FemalePlural(type);
  third_neutral = conjugation.pers3NaturalPlural(type);
  
  if (!first.isEmpty() || !second.isEmpty() || !third_female.isEmpty() ||
      !third_male.isEmpty() || !third_neutral.isEmpty())
  {
    QDomElement plural = m_domDoc.createElement(KVTML_PLURAL);
    
    plural.appendChild(newTextElement(KVTML_1STPERSON, first));
    plural.appendChild(newTextElement(KVTML_2NDPERSON, second));
    
    if (third_common)
    {
      plural.appendChild(newTextElement(KVTML_COMMON, third_female));
    }
    else
    {
      plural.appendChild(newTextElement(KVTML_MALE, third_male));
      plural.appendChild(newTextElement(KVTML_FEMALE, third_female));
      plural.appendChild(newTextElement(KVTML_NEUTRAL, third_neutral));
    }
  }

  return true;
}

QDomElement KEduVocKvtml2Writer::newTextElement(const QString &elementName, const QString &text)
{
  QDomElement retval = m_domDoc.createElement(elementName);
  QDomText textNode = m_domDoc.createTextNode(text);
  retval.appendChild(textNode);
  return retval;
}
