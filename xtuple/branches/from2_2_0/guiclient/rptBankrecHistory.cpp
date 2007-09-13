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

#include "rptBankrecHistory.h"

#include <qvariant.h>
#include <openreports.h>

/*
 *  Constructs a rptBankrecHistory as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
rptBankrecHistory::rptBankrecHistory(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : QDialog(parent, name, modal, fl)
{
    setupUi(this);


    // signals and slots connections
    connect(_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
    connect(_bankaccnt, SIGNAL(newID(int)), this, SLOT(sBankaccntChanged()));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
rptBankrecHistory::~rptBankrecHistory()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void rptBankrecHistory::languageChange()
{
    retranslateUi(this);
}


void rptBankrecHistory::init()
{
  _captive = FALSE;
  
  _bankaccnt->populate("SELECT bankaccnt_id,"
                       "       (bankaccnt_name || '-' || bankaccnt_descrip) "
                       "FROM bankaccnt "
                       "ORDER BY bankaccnt_name;");
  
  sBankaccntChanged();
}

enum SetResponse rptBankrecHistory::set( ParameterList & pParams )
{
  _captive = TRUE;
  
  QVariant param;
  bool valid;
  
  param = pParams.value("bankaccnt_id", &valid);
  if (valid)
    _bankaccnt->setId(param.toInt());
  
  param = pParams.value("bankrec_id", &valid);
  if (valid)
  {
    int bankrecid = param.toInt();
    
    q.prepare( "SELECT bankrec_bankaccnt_id FROM bankrec WHERE (bankrec_id=:bankrecid); " );
    q.bindValue(":bankrecid", bankrecid);
    q.exec();
    if (q.first())
    {
      _bankaccnt->setId(q.value("bankrec_bankaccnt_id").toInt());
      sBankaccntChanged();
    }
    _bankrec->setId(bankrecid);
  }
 
  if (pParams.inList("print"))
  {
    sPrint();
    return NoError_Print;
  }
  
  return NoError;
}

void rptBankrecHistory::sPrint()
{
  ParameterList params;
  params.append("bankaccnt_id", _bankaccnt->id());
  params.append("bankrec_id", _bankrec->id());
  
  orReport report("BankrecHistory", params);
  if(report.isValid())
    report.print();
  else
  {
    report.reportError(this);
    reject();
  }
  
  if (_captive)
    accept();
}

void rptBankrecHistory::sBankaccntChanged()
{
  q.prepare( "SELECT bankrec_id, (formatDate(bankrec_opendate) || '-' || formatDate(bankrec_enddate)) "
             "FROM bankrec "
             "WHERE (bankrec_bankaccnt_id=:bankaccnt_id) "
             "ORDER BY bankrec_opendate, bankrec_enddate; ");
  q.bindValue(":bankaccnt_id", _bankaccnt->id());
  q.exec();
  _bankrec->populate(q);
}
