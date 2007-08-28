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

#include "changePoitemQty.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qvalidator.h>

/*
 *  Constructs a changePoitemQty as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
changePoitemQty::changePoitemQty(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : QDialog(parent, name, modal, fl)
{
    setupUi(this);


    // signals and slots connections
    connect(_change, SIGNAL(clicked()), this, SLOT(sChangeQty()));
    connect(_newQty, SIGNAL(lostFocus()), this, SLOT(sQtyChanged()));
    connect(_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(_po, SIGNAL(valid(bool)), _poitem, SLOT(setEnabled(bool)));
    connect(_po, SIGNAL(newId(int)), this, SLOT(sPopulatePoitem(int)));
    connect(_poitem, SIGNAL(newID(int)), this, SLOT(sPopulate(int)));
    connect(_poitem, SIGNAL(valid(bool)), _change, SLOT(setEnabled(bool)));
    connect(_postComment, SIGNAL(toggled(bool)), _commentGroup, SLOT(setEnabled(bool)));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
changePoitemQty::~changePoitemQty()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void changePoitemQty::languageChange()
{
    retranslateUi(this);
}


void changePoitemQty::init()
{
  _captive = FALSE;
  _cacheFreight = 0.0;

  _po->setType(cPOUnposted | cPOOpen);
  
  _newQty->setValidator(omfgThis->qtyVal());

  _cmnttype->setType(XComboBox::AllCommentTypes);
}

enum SetResponse changePoitemQty::set(ParameterList &pParams)
{
  _captive = TRUE;

  QVariant param;
  bool     valid;

  param = pParams.value("pohead_id", &valid);
  if (valid)
  {
    _po->setId(param.toInt());
    _po->setReadOnly(TRUE);
    _newQty->setFocus();
  }

  param = pParams.value("poitem_id", &valid);
  if (valid)
  {
    q.prepare( "SELECT poitem_pohead_id "
               "FROM poitem "
               "WHERE (poitem_id=:poitem_id);" );
    q.bindValue(":poitem_id", param.toInt());
    q.exec();
    if (q.first())
    {
      _po->setId(q.value("poitem_pohead_id").toInt());
      _po->setReadOnly(TRUE);
      _poitem->setId(param.toInt());
    }

    _newQty->setFocus();
  }

  param = pParams.value("newQty", &valid);
  if (valid)
    _newQty->setText(formatQty(param.toDouble()));

  return NoError;
}

void changePoitemQty::sPopulatePoitem(int pPoheadid)
{
  q.prepare( "SELECT poitem_id,"
             "       ( poitem_linenumber || '-' ||"
             "         COALESCE(item_number, poitem_vend_item_number) || ' (' ||"
             "         COALESCE(item_descrip1, firstLine(poitem_vend_item_descrip)) || ')' ) "
             "FROM poitem LEFT OUTER JOIN "
             "     ( itemsite JOIN item "
             "       ON (itemsite_item_id=item_id)"
             "     ) ON (poitem_itemsite_id=itemsite_id) "
             "WHERE ( (poitem_status <> 'C')"
             " AND (poitem_pohead_id=:pohead_id) ) "
             "ORDER BY poitem_linenumber;" );
  q.bindValue(":pohead_id", pPoheadid);
  q.exec();
  _poitem->populate(q);
}

void changePoitemQty::sPopulate(int pPoitemid)
{
  if (pPoitemid == -1)
  {
    _currentQtyReceived->clear();
    _newQtyReceived->clear();
    _currentQtyOrdered->clear();
    _newQty->clear();
    _currentQtyBalance->clear();
    _newQtyBalance->clear();
    _freight->clear();
  }
  else
  {
    q.prepare( "SELECT poitem_qty_ordered, poitem_qty_received, poitem_qty_returned,"
               "             poitem_freight "
               "FROM poitem "
               "WHERE (poitem_id=:poitem_id);" );
    q.bindValue(":poitem_id", pPoitemid);
    q.exec();
    if (q.first())
    {
      _currentQtyOrdered->setText(formatQty(q.value("poitem_qty_ordered").toDouble()));
      _currentQtyReceived->setText(formatQty(q.value("poitem_qty_received").toDouble()));
      _currentQtyBalance->setText(formatQty((q.value("poitem_qty_ordered").toDouble() - q.value("poitem_qty_received").toDouble())));
      _newQtyReceived->setText(formatQty(q.value("poitem_qty_received").toDouble()));
      _cacheFreight = q.value("poitem_freight").toDouble();
      _freight->setText(formatMoney(q.value("poitem_freight").toDouble()));
      sQtyChanged();
    }
  }
}

void changePoitemQty::sChangeQty()
{
  q.prepare("SELECT changePoitemQty(:poitem_id, :qty);");
  q.bindValue(":poitem_id", _poitem->id());
  q.bindValue(":qty", _newQty->toDouble());
  q.exec();

  if (_postComment->isChecked())
  {
    q.prepare("SELECT postComment(:cmnttype_id, 'PI', :poitem_id, :comment) AS _result");
    q.bindValue(":cmnttype_id", _cmnttype->id());
    q.bindValue(":poitem_id", _poitem->id());
    q.bindValue(":comment", _comment->text());
    q.exec();
  }

  if (_freight->toDouble() != _cacheFreight)
  {
    q.prepare("UPDATE poitem SET poitem_freight=:poitem_freight"
              " WHERE (poitem_id=:poitem_id); ");
    q.bindValue(":poitem_id", _poitem->id());
    q.bindValue(":poitem_freight", _freight->toDouble());
    q.exec();
    // TODO: Check is succedded and display an appropriate error message.
  }
  
  omfgThis->sPurchaseOrdersUpdated(_po->id(), TRUE);
  
  accept();
}

void changePoitemQty::sQtyChanged()
{
  double qtyBalance = (_newQty->toDouble() - _newQtyReceived->text().toDouble());
  if (qtyBalance < 0)
    qtyBalance = 0;

  _newQtyBalance->setText(formatQty(qtyBalance));
}

