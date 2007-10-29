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

#include "openReturnAuthorizations.h"

#include <QVariant>
#include <QMessageBox>
#include <QStatusBar>
#include <QWorkspace>
#include <openreports.h>
#include "returnAuthorization.h"
#include "openReturnAuthorizations.h"
#include "printSoForm.h"
//#include "deliverSalesOrder.h"

/*
 *  Constructs a openReturnAuthorizations as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
openReturnAuthorizations::openReturnAuthorizations(QWidget* parent, const char* name, Qt::WFlags fl)
    : QMainWindow(parent, name, fl)
{
  setupUi(this);

  (void)statusBar();

  // signals and slots connections
  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
  connect(_ra, SIGNAL(populateMenu(QMenu*,QTreeWidgetItem*,int)), this, SLOT(sPopulateMenu(QMenu*)));
  connect(_close, SIGNAL(clicked()), this, SLOT(close()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
  connect(_view, SIGNAL(clicked()), this, SLOT(sView()));
  connect(_ra, SIGNAL(valid(bool)), _view, SLOT(setEnabled(bool)));
  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
  connect(_warehouse, SIGNAL(updated()), this, SLOT(sFillList()));

  statusBar()->hide();
  
  _ra->addColumn(tr("S/O #"),            _orderColumn, Qt::AlignLeft   );
  _ra->addColumn(tr("Cust. #"),          _orderColumn, Qt::AlignLeft   );
  _ra->addColumn(tr("Customer"),         -1,           Qt::AlignLeft   );
  _ra->addColumn(tr("Cust. P/O Number"), _itemColumn,  Qt::AlignLeft   );
  _ra->addColumn(tr("Created"),          _dateColumn,  Qt::AlignCenter );
  _ra->addColumn(tr("Expires"),        _dateColumn,  Qt::AlignCenter );
  
  if (_privleges->check("MaintainSalesOrders"))
  {
    connect(_ra, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
    connect(_ra, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    connect(_ra, SIGNAL(itemSelected(int)), _edit, SLOT(animateClick()));
  }
  else
  {
    _new->setEnabled(FALSE);
    connect(_ra, SIGNAL(itemSelected(int)), _view, SLOT(animateClick()));
  }

  connect(omfgThis, SIGNAL(salesOrdersUpdated(int, bool)), this, SLOT(sFillList()));

  sFillList();
}

/*
 *  Destroys the object and frees any allocated resources
 */
openReturnAuthorizations::~openReturnAuthorizations()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void openReturnAuthorizations::languageChange()
{
    retranslateUi(this);
}

void openReturnAuthorizations::sPrint()
{
  ParameterList params;
  _warehouse->appendValue(params);

  orReport report("ListOpenReturnAuthorizations", params);
  if (report.isValid())
    report.print();
  else
    report.reportError(this);
}

void openReturnAuthorizations::sNew()
{
  ParameterList params;
  params.append("mode", "new");

  returnAuthorization *newdlg = new returnAuthorization();
  newdlg->set(params);
  omfgThis->handleNewWindow(newdlg);
}

void openReturnAuthorizations::sEdit()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("rahead_id", _ra->id());

  returnAuthorization *newdlg = new returnAuthorization();
  newdlg->set(params);
  omfgThis->handleNewWindow(newdlg);
}

void openReturnAuthorizations::sView()
{  
  ParameterList params;
  params.append("mode", "edit");
  params.append("rahead_id", _ra->id());

  returnAuthorization *newdlg = new returnAuthorization();
  newdlg->set(params);
  omfgThis->handleNewWindow(newdlg);
}

void openReturnAuthorizations::sDelete()
{
	/*
  if ( QMessageBox::warning( this, tr("Delete Return Authorization?"),
                             tr("Are you sure that you want to completely delete the selected Sales Order?"),
                             tr("&Yes"), tr("&No"), QString::null, 0, 1 ) == 0 )
  {
    q.prepare("SELECT deleteSo(:sohead_id) AS result;");
    q.bindValue(":sohead_id", _so->id());
    q.exec();
    if (q.first())
    {
      switch (q.value("result").toInt())
      {
        case 0:
          omfgThis->sSalesOrdersUpdated(-1);
          omfgThis->sProjectsUpdated(-1);
          break;

        case -1:
          if ( QMessageBox::information( this, tr("Cannot Delete Sales Order"),
                                         tr( "The selected Sales Order cannot be deleted as there have been shipments posted against it.\n"
                                             "Would you like to Close the selected Sales Order instead?" ),
                                         tr("Yes"), tr("No"), QString::null, 0, 1 ) == 0 )
          {
            q.prepare( "UPDATE raitem "
                       "SET raitem_status='C' "
                       "WHERE ((raitem_status <> 'X') AND (raitem_cohead_id=:sohead_id));" );
            q.bindValue(":sohead_id", _so->id());
            q.exec();
      
            sFillList();
          }

          break;

        case -2:
          QMessageBox::information( this, tr("Cannot Delete Sales Order"),
                                    tr( "The selected Sales Order cannot be deleted as shipping stock has been posted against one or\n"
                                        "more of its line items.  You must return this stock before you may delete this Sales Order." ) );
          break;
//  ToDo
      }
    }
  }
  */
}

void openReturnAuthorizations::sPopulateMenu(QMenu *pMenu)
{
  int menuItem;

  menuItem = pMenu->insertItem(tr("Edit..."), this, SLOT(sEdit()), 0);
  if (!_privleges->check("MaintainReturns"))
    pMenu->setItemEnabled(menuItem, FALSE);

  menuItem = pMenu->insertItem(tr("View..."), this, SLOT(sView()), 0);

  menuItem = pMenu->insertItem(tr("Delete..."), this, SLOT(sDelete()), 0);
  if (!_privleges->check("MaintainReturns"))
    pMenu->setItemEnabled(menuItem, FALSE);

  pMenu->insertSeparator();

  /*
  if (_metrics->boolean("EnableBatchManager"))
  {
    menuItem = pMenu->insertItem(tr("Schedule S/O for Email Delivery..."), this, SLOT(sDeliver()), 0);
  }*/

  menuItem = pMenu->insertItem(tr("Print Return Authorization Form..."), this, SLOT(sPrintForms()), 0); 

}


void openReturnAuthorizations::sFillList()
{
  QString sql( "SELECT DISTINCT rahead_id, rahead_number,"
               "       COALESCE(cust_number, :error),"
               "       rahead_billtoname, rahead_custponumber,"
               "       formatDate(rahead_authdate) AS f_ordered,"
               "       formatDate(raitem_expiredate)) AS f_expires "
               "FROM rahead LEFT OUTER JOIN cust ON (rahead_cust_id=cust_id) "
               "     LEFT OUTER JOIN raitem JOIN itemsite ON (raitem_itemsite_id=itemsite_id) "
               "     ON (raitem_rahead_id=rahead_id) "
               "WHERE (((raitem_status = 'O') OR (raitem_status IS NULL)) ");

  if (_warehouse->isSelected())
    sql += " AND (itemsite_warehous_id=:warehous_id)";

  sql += " ) "
         "GROUP BY rahead_id, rahead_number, cust_number, rahead_billtoname,"
         "         rahead_custponumber, rahead_orderdate "
         "ORDER BY rahead_number ";

  q.prepare(sql);
  _warehouse->bindValue(q);
  q.bindValue(":error", tr("Error"));
  q.exec();

  _ra->populate(q);
  _ra->setDragString("soheadid=");
}
/*
void openReturnAuthorizations::sDeliver()
{
  ParameterList params;
  params.append("sohead_id", _so->id());

  deliverSalesOrder newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}
*/


void openReturnAuthorizations::sPrintForms()
{
  ParameterList params;
  params.append("rahead_id", _ra->id());

  printSoForm newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

