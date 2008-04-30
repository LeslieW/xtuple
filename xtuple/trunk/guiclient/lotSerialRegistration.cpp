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
 * (c) 1999-2008 OpenMFG, LLC, d/b/a xTuple. All Rights Reserved. 
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
 * Copyright (c) 1999-2008 by OpenMFG, LLC, d/b/a xTuple
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

#include "lotSerialRegistration.h"

#include <QMessageBox>
#include <QSqlError>
#include <QVariant>
#include <QMenu>
#include <QSqlRecord>

#include "storedProcErrorLookup.h"
#include "characteristicAssignment.h"

/*
 *  Constructs a lotSerialRegistration as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
lotSerialRegistration::lotSerialRegistration(QWidget* parent, const char* name, Qt::WFlags fl)
    : XMainWindow(parent, name, fl)
{
  setupUi(this);

  _lsregid = -1;

  // signals and slots connections
  connect(_save,	SIGNAL(clicked()),	this,	SLOT(sSave()));
  connect(_soldDate, SIGNAL(newDate(const QDate&)), this, SLOT(sDateUpdated()));
 
  _lotSerial->setStrict(true);
  _cntct->setAccountVisible(FALSE);
  _cntct->setActiveVisible(FALSE);

  resize(minimumSize());
}

/*
 *  Destroys the object and frees any allocated resources
 */
lotSerialRegistration::~lotSerialRegistration()
{
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void lotSerialRegistration::languageChange()
{
  retranslateUi(this);
}

enum SetResponse lotSerialRegistration::set(const ParameterList &pParams)
{
  QVariant param;
  bool     valid;

  param = pParams.value("ls_id", &valid);
  if (valid)
    _lotSerial->setId(param.toInt());
  
  param = pParams.value("number", &valid);
  if (valid)
    _number=param.toString();

  param = pParams.value("mode", &valid);
  if (valid)
  {
    _mode = cNew;

    if (param.toString() == "new")
      _mode = cNew;
    else if (param.toString() == "edit")
    {
      _mode = cEdit;
      populate();
      _save->setFocus();
    }
    else if (param.toString() == "view")
    {
      _mode = cView;
      _regDate->setEnabled(false);
      _soldDate->setEnabled(false);
      _expireDate->setEnabled(false);
      _crmacct->setEnabled(false);
      _cntct->setEnabled(false);
      _type->setEnabled(false);
      _item->setReadOnly(true);
      _lotSerial->setEnabled(false);
      _newChar->setEnabled(false);
      _editChar->setEnabled(false);
      _deleteChar->setEnabled(false);
      _notes->setEnabled(false);
      _save->hide();
      _cancel->setText(tr("&Close"));
      _cancel->setFocus();
    }
  }

  param = pParams.value("crmacct_id", &valid);
  if (valid)
  {
    _crmacct->setId(param.toInt());
    _crmacct->setEnabled(false);
  }
  return NoError;
}

void lotSerialRegistration::sNewCharass()
{
  ParameterList params;
  params.append("mode", "new");
  params.append("lsreg_id", _lotSerial->id());

  characteristicAssignment newdlg(this, "", TRUE);
  newdlg.set(params);

  if (newdlg.exec() != XDialog::Rejected)
    sFillList();
}

void lotSerialRegistration::sEditCharass()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("charass_id", _charass->id());

  characteristicAssignment newdlg(this, "", TRUE);
  newdlg.set(params);

  if (newdlg.exec() != XDialog::Rejected)
    sFillList();
}

void lotSerialRegistration::sDeleteCharass()
{
  q.prepare( "DELETE FROM charass "
             "WHERE (charass_id=:charass_id);" );
  q.bindValue(":charass_id", _charass->id());
  q.exec();
  if (q.lastError().type() != QSqlError::None)
  {
    systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
    return;
  }

  sFillList();
}

void lotSerialRegistration::sFillList()
{
  // TODO
}

void lotSerialRegistration::populate()
{
  q.prepare("SELECT *"
            "  FROM lsreg LEFT OUTER JOIN ls ON (lsreg_ls_id=ls_id)"
            " WHERE(lsreg_number=:number);");
  q.bindValue(":number", _number);
  q.exec();
  if(q.first())
  {
    _lsregid = q.value("lsreg_id").toInt();
    _regNumber->setText(q.value("lsreg_number").toString());
    _type->setId(q.value("lsreg_regtype_id").toInt());
    _lotSerial->setId(q.value("lsreg_ls_id").toInt());
    _qty->setText(q.value("lsreg_qty").toString());
    _item->setId(q.value("ls_item_id").toInt());
    _regDate->setDate(q.value("lsreg_regdate").toDate());
    _soldDate->setDate(q.value("lsreg_solddate").toDate());
    _expireDate->setDate(q.value("lsreg_expiredate").toDate());
    _crmacct->setId(q.value("lsreg_crmacct_id").toInt());
    _cntct->setId(q.value("lsreg_cntct_id").toInt());
    _notes->setText(q.value("lsreg_notes").toString());
    if(!q.value("lsreg_cohead_id").isNull())
      _so->setId(q.value("lsreg_cohead_id").toInt());
    if(!q.value("lsreg_shiphead_id").isNull())
      _shipment->setId(q.value("lsreg_shiphead_id").toInt());
  }
  else if(q.lastError().type() != QSqlError::None)
  {
    systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
    return;
  }
}

void lotSerialRegistration::sSave()
{
  if(cView == _mode)
    return;

  // TODO: Add any error checking that is necessary here

  if(cNew == _mode)
  {
    q.exec("SELECT NEXTVAL('lsreg_lsreg_id_seq') AS _lsreg_id;");
    if (q.first())
      _lsregid = q.value("_lsreg_id").toInt();
    else if (q.lastError().type() != QSqlError::None)
    {
      systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
      return;
    }
    q.prepare("INSERT INTO lsreg"
              "      (lsreg_id, lsreg_number, lsreg_regtype_id,"
              "       lsreg_ls_id, lsreg_qty, lsreg_regdate, lsreg_solddate,"
              "       lsreg_expiredate, lsreg_crmacct_id, lsreg_cntct_id,"
              "       lsreg_notes, lsreg_cohead_id, lsreg_shiphead_id) "
              "VALUES(:lsreg_id, :lsreg_number, :lsreg_regtype_id,"
              "       :lsreg_ls_id, :lsreg_qty, :lsreg_regdate, :lsreg_solddate,"
              "       :lsreg_expiredate, :lsreg_crmacct_id, :lsreg_cntct_id,"
              "       :lsreg_notes, :lsreg_cohead_id, :lsreg_shiphead_id);");
  }
  else if(cEdit == _mode)
    q.prepare("UPDATE lsreg"
              "   SET lsreg_number=:lsreg_number,"
              "       lsreg_regtype_id=:lsreg_regtype_id,"
              "       lsreg_ls_id=:lsreg_ls_id,"
              "       lsreg_qty=:lsreg_qty,"
              "       lsreg_regdate=:lsreg_regdate,"
              "       lsreg_solddate=:lsreg_solddate,"
              "       lsreg_expiredate=:lsreg_expiredate,"
              "       lsreg_crmacct_id=:lsreg_crmacct_id,"
              "       lsreg_cntct_id=:lsreg_cntct_id,"
              "       lsreg_notes=:lsreg_notes,"
              "       lsreg_cohead_id=:lsreg_cohead_id,"
              "       lsreg_shiphead_id=:lsreg_shiphead_id"
              " WHERE(lsreg_id=:lsreg_id);");
  
  q.bindValue(":lsreg_id", _lsregid);
  q.bindValue(":lsreg_number", _regNumber->text().stripWhiteSpace());
  q.bindValue(":lsreg_regtype_id", _type->id());
  q.bindValue(":lsreg_ls_id", _lotSerial->id());
  q.bindValue(":lsreg_qty", _qty->toDouble());
  q.bindValue(":lsreg_regdate", _regDate->date());
  q.bindValue(":lsreg_solddate", _soldDate->date());
  q.bindValue(":lsreg_expiredate", _expireDate->date());
  q.bindValue(":lsreg_crmacct_id", _crmacct->id());
  q.bindValue(":lsreg_cntct_id", _cntct->id());
  q.bindValue(":lsreg_notes", _notes->text());
  if(_so->id() != -1)
    q.bindValue(":lsreg_cohead_id", _so->id());
  if(_shipment->id() != -1)
    q.bindValue(":lsreg_shiphead_id", _shipment->id());

  q.exec();
  if (q.lastError().type() != QSqlError::None)
  {
    systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
    return;
  }

  close();
}

void lotSerialRegistration::sDateUpdated()
{
  if(cView == _mode)
    return;

  QDate date = _soldDate->date();
  if(date.isNull())
    return;

  q.prepare("SELECT item_warrdays"
            "  FROM item"
            " WHERE(item_id=:item_id);");
  q.bindValue(":item_id", _item->id());
  q.exec();
  if(q.first())
  {
    date.addDays(q.value("item_warrdays").toInt());
    _expireDate->setDate(date);
  }
}
