/*
 * Common Public Attribution License Version 1.0. 
 * 
 * The contents of this file are subject to the Common Public Attribution 
 * License Version 1.0 (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License 
 * at http://www.xTuple.com/CPAL.  The License is based on the Mozilla 
 * Public License Version 1.1 but Sections 14 and 15 have been added to 
 * cover use of software over a computer network and provide for limited 
 * attribution for the Original Developer. In addition, Exhibit A has 
 * been modified to be consistent with Exhibit B.
 * 
 * Software distributed under the License is distributed on an "AS IS" 
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See 
 * the License for the specific language governing rights and limitations 
 * under the License. 
 * 
 * The Original Code is PostBooks Accounting, ERP, and CRM Suite. 
 * 
 * The Original Developer is not the Initial Developer and is __________. 
 * If left blank, the Original Developer is the Initial Developer. 
 * The Initial Developer of the Original Code is OpenMFG, LLC, 
 * d/b/a xTuple. All portions of the code written by xTuple are Copyright 
 * (c) 1999-2007 OpenMFG, LLC, d/b/a xTuple. All Rights Reserved. 
 * 
 * Contributor(s): ______________________.
 * 
 * Alternatively, the contents of this file may be used under the terms 
 * of the xTuple End-User License Agreeement (the xTuple License), in which 
 * case the provisions of the xTuple License are applicable instead of 
 * those above.  If you wish to allow use of your version of this file only 
 * under the terms of the xTuple License and not to allow others to use 
 * your version of this file under the CPAL, indicate your decision by 
 * deleting the provisions above and replace them with the notice and other 
 * provisions required by the xTuple License. If you do not delete the 
 * provisions above, a recipient may use your version of this file under 
 * either the CPAL or the xTuple License.
 * 
 * EXHIBIT B.  Attribution Information
 * 
 * Attribution Copyright Notice: 
 * Copyright (c) 1999-2007 by OpenMFG, LLC, d/b/a xTuple
 * 
 * Attribution Phrase: 
 * Powered by PostBooks, an open source solution from xTuple
 * 
 * Attribution URL: www.xtuple.org 
 * (to be included in the "Community" menu of the application if possible)
 * 
 * Graphic Image as provided in the Covered Code, if any. 
 * (online at www.xtuple.com/poweredby)
 * 
 * Display of Attribution Information is required in Larger Works which 
 * are defined in the CPAL as a work which combines Covered Code or 
 * portions thereof with code not governed by the terms of the CPAL.
 */

#ifndef CREDITCARDPROCESSOR_H
#define CREDITCARDPROCESSOR_H

#include <QDomDocument>
#include <QHash>
#include <QObject>
#include <QString>

class CreditCardProcessor : public QObject
{
  Q_OBJECT

  public:
    virtual ~CreditCardProcessor();

    // no public constructor for abstract class, just a factory
    static CreditCardProcessor	*getProcessor();

    // these are the primary transaction handlers and should not be overridden:
    virtual int	    authorize(const int, const int, const double, const int, QString&, int&, QString, int&);
    virtual int	    charge(const int, const int, const double, const int, QString&, QString&, int&, QString, int&);
    virtual int	    chargePreauthorized(const int, const double, const int, QString&, QString&, int&);
    virtual int	    credit(const int, const int, const double, const int, QString&, QString&, int&);
    virtual int	    voidPrevious(const int&);

    // these are support methods that typically won't be overridden
    virtual int	    checkConfiguration();
    virtual int	    currencyId();
    virtual QString currencyName();
    virtual QString currencySymbol();
    virtual bool    isLive();
    virtual bool    isTest();
    static  QString errorMsg();		// most recent error
    static  QString errorMsg(const int);

  protected:
    CreditCardProcessor();

    // do* handle the processor-specific processing and are expected to be overridden
    virtual int doAuthorize(const int, const int, const double, const int, QString&, QString&, int&);
    virtual int doCharge(const int, const int, const double, const int, QString&, QString&, int&);
    virtual int doCheckConfiguration();
    virtual int doChargePreauthorized(const int, const int, const double, const int, QString&, QString&, int&);
    virtual int doCredit(const int, const int, const double, const int, QString&, QString&, int&);
    virtual int doVoidPrevious(const int);

    virtual int checkCreditCard(const int, const int, QString&);
    virtual int checkCreditCardProcessor()	{ return false; };
    virtual int processXML(const QDomDocument&, QDomDocument&);

    int			_currencyId;
    QString		_currencyName;
    QString		_currencySymbol;
    static QString	_errorMsg;
    static QHash<int, QString>	_msgHash;
    QString		_plogin;
    QString		_ppassword;
    QString		_pport;
    QString		_pserver;

  private:
    static CreditCardProcessor *_processor;
};

#endif // CREDITCARDPROCESSOR_H
